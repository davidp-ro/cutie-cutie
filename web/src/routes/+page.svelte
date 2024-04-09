<script lang="ts">
  import DataCard from "$lib/components/DataCard.svelte";
  import * as Card from "$lib/components/ui/card/index.js";
  import { Button } from "$lib/components/ui/button";

  import { MessageHandler } from "$lib/messageParser";
  import type { EspBroadcastMessagePacket } from "$lib/types";
  import { onMount } from "svelte";
  import Slider from "$lib/components/ui/slider/slider.svelte";

  let socket: WebSocket;
  let otherMessages: string[] = [];

  let lastSentFanValue = -1;
  let sliderValue: number[] = [-1];

  let currentPacket: EspBroadcastMessagePacket | null = null;
  let receivedAt: Date | null = null;
  let lastReceivedSeconds = 0;

  let timer: any;

  const debounceSliderChange = () => {
    clearTimeout(timer);
    timer = setTimeout(handleSliderChange, 750);
  };

  $: {
    if (sliderValue[0] !== -1) {
      debounceSliderChange();
    }
  }

  const sendMessage = () => {
    socket.send(MessageHandler.build("1", "fan", 64));
  };

  const onMessage = (msg: string) => {
    if (msg.startsWith("e:data")) {
      currentPacket = MessageHandler.parse(msg);
      receivedAt = new Date();

      if (sliderValue[0] === -1) {
        sliderValue = [currentPacket?.fan ?? -1];
      }
    } else {
      otherMessages = [...otherMessages, msg];
    }
  };

  const mapValue = (val: number, range: [number, number]) => {
    const [min, max] = range;
    return Math.floor(((val - min) / (max - min)) * 100);
  };

  const mapLightVal = (val: number) => {
    if (val > 650) return "🌚";
    return "😎";
  };

  const handleSliderChange = () => {
    const value = sliderValue[0];
    if (value === lastSentFanValue) return;

    lastSentFanValue = value;

    // The value to be sent should be mapped from 0-100 to 0-255
    let sendVal: number;
    if (value === 0) {
      sendVal = 0;
    } else {
      sendVal = Math.floor((value / 100) * 255);
    }

    socket.send(MessageHandler.build("1", "fan", sendVal));
  };

  onMount(() => {
    socket = new WebSocket("ws://localhost:8080");

    socket.onopen = () => {
      console.log("Connected to server");
    };

    socket.onmessage = (event) => onMessage(event.data);

    socket.onclose = () => {
      console.log("Disconnected from server, retrying to connect...");
    };

    setInterval(() => {
      const now = new Date();
      const diff = now.getTime() - (receivedAt?.getTime() ?? now.getTime());
      lastReceivedSeconds = Math.floor(diff / 1000);
    }, 500);
  });
</script>

<!-- <button class="px-2 py-1 rounded-lg bg-gray-100" on:click={sendMessage}>
  Send test message
</button> -->

{#if !currentPacket}
  <Card.Root class="mt-8 w-full max-w-lg mx-auto">
    <Card.Header>
      <Card.Title>Waiting for data</Card.Title>
      <Card.Description>Listening for data from the ESP</Card.Description>
    </Card.Header>
  </Card.Root>
{:else}
  <Card.Root class="mt-8 w-full max-w-[80vw] xl:max-w-[60vw] mx-auto">
    <Card.Header>
      <Card.Title>Live Data</Card.Title>
      <Card.Description>
        Last received {lastReceivedSeconds < 2
          ? "< 1 second"
          : `${lastReceivedSeconds} seconds`} ago.
      </Card.Description>
    </Card.Header>
    <Card.Content>
      <div class="flex gap-2 items-center">
        <DataCard name="Temperature" value="{currentPacket.temperature} °C" />
        <DataCard name="Humidity" value="{currentPacket.humidity} %" />
        <DataCard
          name="Light Level"
          value={mapLightVal(currentPacket.lightLevel)}
        />
        <DataCard name="Relay" value={currentPacket.relay ? "On" : "Off"} />
        <DataCard
          name="Fan"
          value="{mapValue(currentPacket.fan, [0, 255])} %"
        />
      </div>

      <div class="mt-4 mx-4 flex gap-6 items-center">
        <Button
          on:click={() => {
            socket.send(
              MessageHandler.build(
                "1",
                "relay",
                currentPacket?.relay ?? 0 > 0 ? 0 : 1
              )
            );
          }}
        >
          Turn {currentPacket.relay > 0 ? "off" : "on"} the light
        </Button>

        <div class="inline-flex gap-2 items-center w-full">
          <p class="whitespace-nowrap">Fan Speed:</p>
          <Slider bind:value={sliderValue} min={0} max={100} step={1} />
        </div>
      </div>
    </Card.Content>
  </Card.Root>
{/if}
