export type EspBroadcastMessageType = keyof EspBroadcastMessagePacket;

export type EspBroadcastMessagePacket = {
  espID: string;

  /** float */
  temperature: number;

  /** float */
  pressure: number;

  /** int, 0...100 */
  humidity: number;

  /** int, 0...255 */
  lightLevel: number;

  /** int, 0/1 */
  relay: number;

  /** int, 0...255 */
  fan: number;
};
