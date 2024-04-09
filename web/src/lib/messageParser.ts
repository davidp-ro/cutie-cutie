import type {
  EspBroadcastMessagePacket,
  EspBroadcastMessageType,
} from "./types";

export class MessageHandler {
  /**
   * Parse a message from the ESP into a packet
   *
   * @param msg e:data:esp_id:value:value...
   */
  static parse(msg: string): EspBroadcastMessagePacket | null {
    const split = msg.split(":");

    try {
      return {
        espID: split[2],
        temperature: parseFloat(split[3]),
        pressure: parseFloat(split[4]),
        humidity: parseInt(split[5]),
        lightLevel: parseInt(split[6]),
        relay: parseInt(split[7]),
        fan: parseInt(split[8]),
      };
    } catch (e) {
      console.error("Failed to parse message", msg, e);
      return null;
    }
  }

  /**
   * Build a message to send to the ESP
   *
   * @param cmdType the type of the command
   * @param cmdValue the value, must match values in ranges
   */
  static build(
    espId: string,
    cmdType: EspBroadcastMessageType,
    cmdValue: number
  ): string {
    return `w:set:${espId}:${cmdType}:${cmdValue}`;
  }
}
