"use client";

import { useState, useEffect } from "react";

const OFFLINE_THRESHOLD_MS = 3 * 60 * 1000;

const DEVICE_ID = "greenhouse_board_1";

export default function Dashboard() {
  const [lastHeartbeat, setLastHeartbeat] = useState<Date | null>(null);
  const [isOnline, setIsOnline] = useState<boolean>(false);
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    const checkDeviceStatus = async () => {
      try {
        const res = await fetch(`/api/heartbeat?id=${DEVICE_ID}`, {
          cache: "no-store",
        });

        if (!res.ok) throw new Error("Failed to fetch status");

        const data = await res.json();

        if (data.timestamp) {
          const lastTime = new Date(data.timestamp);
          setLastHeartbeat(lastTime);

          const timeSinceLastPing = Date.now() - lastTime.getTime();
          setIsOnline(timeSinceLastPing < OFFLINE_THRESHOLD_MS);
        } else {
          setIsOnline(false);
        }
      } catch (err) {
        console.error("Error polling hardware status:", err);
        setIsOnline(false);
      } finally {
        setLoading(false);
      }
    };

    checkDeviceStatus();
    const interval = setInterval(checkDeviceStatus, 5000);
    return () => clearInterval(interval);
  }, []);

  if (loading) {
    return (
      <div className="p-6 text-center text-gray-500">
        Loading system layout...
      </div>
    );
  }

  const time = new Intl.DateTimeFormat("en-US", {
    hour: "numeric",
    minute: "numeric",
    hour12: true,
  }).format(lastHeartbeat?.getDate());
  return (
    <div className="flex flex-row gap-4 items-baseline">
      <p className="text-3xl">Dashboard</p>
      <p>Status: {isOnline ? "Online" : "offline"} </p>
      <p>Last Heartbeat: {time}</p>
    </div>
  );
}
