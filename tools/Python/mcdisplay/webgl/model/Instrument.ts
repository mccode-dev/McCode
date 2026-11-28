import { Component } from "./Component";

export interface Instrument {
  name: string;
  abspath: string;
  params: string;
  params_defaults: string;
  params_values: string;
  cmd: string;
  components: Component[];
  monitors?: Record<string, any>; // --simdir monitor data, by component name
}
