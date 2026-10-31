import type { Metadata } from "next";
import type { ReactNode } from "react";
import NavigationSideBar from "../src/navigation/NavigationSideBar";
import "./globals.css";

export const metadata: Metadata = {
  title: "Greenhouse",
  description: "A fresh start for the Greenhouse app.",
};

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html lang="en">
      <body>
        <div className="flex h-screen w-full flex-row">
          <NavigationSideBar />
          <main>{children}</main>
        </div>
      </body>
    </html>
  );
}
