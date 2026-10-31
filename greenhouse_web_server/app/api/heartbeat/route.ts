import { NextResponse } from 'next/server';
import { redisHelper } from '../../../lib/redis';

export const dynamic = 'force-dynamic';

export async function POST(request: Request) {
  try {
    const apiKey = request.headers.get('x-api-key');


   if (apiKey !== process.env.API_ROUTE_SECRET) {
      return NextResponse.json({ message: 'Unauthorized' }, { status: 401 });
    }

    const body = await request.json();
    const { deviceId } = body;

    if (!deviceId) {
      return NextResponse.json({ error: 'Missing deviceId' }, { status: 400 });
    }
    const currentTimestamp = Date.now();
    await redisHelper.set(`hardware:heartbeat:${deviceId}`, currentTimestamp);

    return NextResponse.json({ success: true, saved: currentTimestamp });
  } catch (error) {
    return NextResponse.json({ error: 'Invalid request layout' }, { status: 400 });
  }
}


export async function GET(request: Request) {
  const { searchParams } = new URL(request.url);
  const deviceId = searchParams.get('id');

  if (!deviceId) {
    return NextResponse.json({ error: 'Missing device id parameter' }, { status: 400 });
  }
  
  const lastTimestamp: number | null = await redisHelper.get(`hardware:heartbeat:${deviceId}`);

  return NextResponse.json({ 
    deviceId,
    timestamp: lastTimestamp 
  });
}