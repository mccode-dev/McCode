import { Instrument } from "../model/Instrument";

const colors = [
  "#F80000",
  "#00F800",
  "#0000F8",
  "#00F8F8",
  "#F800F8",
  "#00F880",
  "#F8F800",
  "#F88000",
  "#80F800",
  "#0080F8",
  "#8000F8",
  "#F80080",
  "#A8A8A8",
];

export function initializeInstrument(data: any): Instrument {
  return {
    ...data,
    components: data.components.map((component, index) => ({
      id: index,
      name: component.name,
      m4: component.m4,
      drawcalls: component.drawcalls,
      color: colors[index % colors.length], // Assigning color from the colors array
      transparency: 0.4, // as webgl-classic: translucent, so hollow cavities and enclosed parts show
    })),
  };
}
