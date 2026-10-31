import type { NextConfig } from 'next';

const config: NextConfig = {
  agentRules: false,
  turbopack: { root: process.cwd() },
  experimental: {
    turbopackFileSystemCacheForDev: false,
  },
 async redirects() {
    return [
      {
        source: '/',
        destination: '/dashboard',
        permanent: false, // Set to true if this change is permanent (308 redirect)
      },
    ];
  },
};

export default config;
