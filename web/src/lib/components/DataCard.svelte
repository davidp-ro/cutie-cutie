<script lang="ts">
  import * as Card from "$lib/components/ui/card/index.js";

  export let name: string;
  export let value: any;
</script>

<Card.Root class="w-full">
  <Card.Header>
    <Card.Title>{name}</Card.Title>
  </Card.Header>
  <Card.Content>
    <p class="text-medium text-lg">
      {value}
    </p>
  </Card.Content>
</Card.Root>
