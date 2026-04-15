'use client';

import { useState } from 'react';
import Script from 'next/script';

declare global {
  namespace JSX {
    interface IntrinsicElements {
      'esp-web-install-button': any;
    }
  }
}

export default function InstallerPage() {
  const [room, setRoom] = useState('');
  const [manifestUrl, setManifestUrl] = useState<string | null>(null);
  const [isReady, setIsReady] = useState(false);
  const [showHelp, setShowHelp] = useState(false);

  const handlePrepareFlash = async() => {
    if (!room) return;

    setIsReady(true);

    await fetch(
      `${process.env.NEXT_PUBLIC_BACKEND_URL}/build?room=${encodeURIComponent(room)}`
    );

    const manifest = {
      name: `CrowPanel - ${room}`,
      builds: [{
        chipFamily: "ESP32-S3",
        parts: [
          { path: `${process.env.NEXT_PUBLIC_BACKEND_URL}/firmware`, offset: 0x10000 },
          { path: `${process.env.NEXT_PUBLIC_BACKEND_URL}/bootloader`, offset: 0x0 },
          { path: `${process.env.NEXT_PUBLIC_BACKEND_URL}/partitions`, offset: 0x8000 },
          { path: `${process.env.NEXT_PUBLIC_BACKEND_URL}/boot_app`, offset: 0xE000 }
        ]
      }]
    };

    const blob = new Blob([JSON.stringify(manifest)], { type: 'application/json' });
    const newManifestUrl = URL.createObjectURL(blob);

    setManifestUrl(newManifestUrl);
  };

  return (
    <div className="min-h-screen bg-gray-900 flex flex-col items-center justify-center py-12 px-4 sm:px-6 lg:px-8 font-sans">

      <Script
        type="module"
        src="https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module"
        strategy="lazyOnload"
      />

      <div className="max-w-lg w-full space-y-6 bg-gray-800 p-8 rounded-xl shadow-2xl border border-gray-700 text-center">

        <h1 className="text-2xl font-bold text-white tracking-tight">
          Welcome to the CrowPanel web installer!
        </h1>

        <div className="text-gray-300 text-base space-y-4">
          <p>
            Plug in your ESP to a USB port. We will install the custom firmware for your selected room to it.
          </p>

          <p>
            Hit <span className="font-semibold text-white">"Prepare & Install"</span> and select the correct COM port.{' '}
            <button
              onClick={() => setShowHelp(!showHelp)}
              className="text-blue-400 hover:text-blue-300 underline transition-colors focus:outline-none"
            >
              No device found?
            </button>
          </p>

          {showHelp && (
            <div className="mt-4 p-4 bg-gray-700 rounded-lg text-sm text-left text-gray-200 border border-gray-600 animate-fade-in-up">
              <p className="mb-2"><span className="font-bold text-red-400">Linux users:</span> If your device is not showing up, you likely have a permissions issue with your serial port.</p>
              <p className="mb-2">Open your terminal and grant read/write access by running:</p>
              <pre className="bg-black p-3 rounded-md text-green-400 font-mono text-xs overflow-x-auto border border-gray-600">
                sudo chmod a+rw [port]
              </pre>
              <p className="mt-2 text-xs text-gray-400">
                (Replace <code className="bg-gray-900 px-1 py-0.5 rounded text-yellow-300">[port]</code> with your actual port, e.g., <code className="bg-gray-900 px-1 py-0.5 rounded text-yellow-300">/dev/ttyUSB0</code>)
              </p>
              <div className="flex justify-center">
                <p className="text-sm font-medium text-yellow-500 bg-yellow-500/10 py-2 mt-8 px-3 rounded-md border border-yellow-500/20 inline-block">
                  ⚠️ Make sure your USB cable supports data transfer.
                </p>
              </div>
            </div>
          )}

          <p className="text-blue-400 font-medium pt-2">
            Get your display installed and connected in less than 3 minutes!
          </p>
        </div>

        <div className="mt-8 pt-6 border-t border-gray-700 space-y-6">
          <div className="text-left">
            <select
              id="room-select"
              value={room}
              onChange={(e) => {
                setRoom(e.target.value);
                setIsReady(false);
                setManifestUrl(null);
              }}
              className="block w-full pl-3 pr-10 py-2.5 text-base border-gray-600 focus:outline-none focus:ring-2 focus:ring-blue-500 rounded-md bg-gray-900 text-white transition-colors cursor-pointer"
            >
              <option value="">-- Select a room first --</option>
              <option value="inn011">INN 011</option>
              <option value="INN033">INN 033</option>
              <option value="inn041">INN 041</option>
            </select>
          </div>

          {!isReady ? (
            <button
              onClick={handlePrepareFlash}
              disabled={!room}
              className={`w-full flex justify-center py-3 px-4 rounded-md shadow-sm text-base font-bold text-white transition-all focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500 focus:ring-offset-gray-900 ${
                room
                  ? 'bg-blue-600 hover:bg-blue-500'
                  : 'bg-gray-700 text-gray-400 cursor-not-allowed'
              }`}
            >
              Prepare & Install
            </button>
          ) : (
            <div className="flex flex-col items-center space-y-4 animate-fade-in-up">

              <div className="flex justify-center min-h-[40px] w-full p-4 bg-gray-900 rounded-md border border-gray-700">
                {manifestUrl ? (
                  <div className="flex flex-col items-center">
                    <p className="text-sm text-gray-400 mb-3">Firmware ready. Click below to connect.</p>
                    <esp-web-install-button manifest={manifestUrl}></esp-web-install-button>
                  </div>
                ) : (
                  <p className="text-sm text-gray-400">Loading component...</p>
                )}
              </div>

              <button
                onClick={() => { setIsReady(false); setManifestUrl(null); }}
                className="text-sm text-gray-500 hover:text-white transition-colors underline focus:outline-none"
              >
                Cancel / Restart
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
