import { Redis as UpstashRedis } from '@upstash/redis';
import { createClient } from 'redis';

let localRedisClient: any = null;
let upstashRedisClient: any = null;

const isDevelopment = process.env.NODE_ENV === 'development';

if (isDevelopment) {
  if (!global.globalLocalRedis) {
    global.globalLocalRedis = createClient({
      url: process.env.REDIS_LOCAL_URL,
    });
    global.globalLocalRedis.connect().catch(console.error);
  }
  localRedisClient = global.globalLocalRedis;
} else {
  upstashRedisClient = UpstashRedis.fromEnv();
}

export const redisHelper = {
  async set(key: string, value: any): Promise<void> {
    if (isDevelopment) {
      await localRedisClient.set(key, JSON.stringify(value));
    } else {
      await upstashRedisClient.set(key, value);
    }
  },

  async get(key: string): Promise<any> {
    if (isDevelopment) {
      const data = await localRedisClient.get(key);
      return data ? JSON.parse(data) : null;
    } else {
      return await upstashRedisClient.get(key);
    }
  }
};

declare global {
  var globalLocalRedis: any;
}
