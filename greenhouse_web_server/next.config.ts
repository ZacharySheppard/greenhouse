import type { NextConfig } from 'next';

const config: NextConfig = {
  agentRules: false,
  turbopack: { root: process.cwd() },
  experimental: {
  turbopackFileSystemCacheForDev: false,
}
};

export default config;
