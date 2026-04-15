import "./globals.css";

export const metadata = {
  title: "CrowPanel Installer",
  description: "Firmware deployment for CrowPanel E-Ink displays",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className="bg-gray-900 w-full h-full text-white selection:bg-blue-500 selection:text-white">
        {children}
      </body>
    </html>
  );
}
