import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

// instrument.json / particles.json are per-instrument-run outputs written
// directly into the served output directory by mcdisplay.py's
// write_browse() - they don't exist at build time, so they're not part of
// this build config (a previous vite-plugin-static-copy step here pointed
// at placeholder paths that were never real files and was removed).
export default defineConfig({
  plugins: [react()],
  server: {
    hmr: {
      overlay: false,
    },
  },
});
