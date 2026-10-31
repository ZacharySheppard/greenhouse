import { type LucideIcon, HomeIcon, FileText } from "lucide-react";

import Link from "next/link";

interface NavigationSideBarItemProps {
  icon: LucideIcon;
  label: string;
}

const NavigationSideBarItem = ({
  icon: Icon,
  label,
}: NavigationSideBarItemProps) => {
  return (
    <Link
      href={label.toLowerCase()}
      className="flex flex-row items-center gap-4 rounded-lg px-3 py-2 text-garden-cream transition-colors hover:bg-garden-leaf focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-garden-clay"
      aria-label={label}
    >
      <Icon size={20}></Icon>
      <p>{label}</p>
    </Link>
  );
};

const NavigationItems = () => {
  const navigationItems = [
    { icon: HomeIcon, label: "Dashboard" },
    { icon: FileText, label: "Blog" },
  ];
  return (
    <div className="flex flex-col gap-3">
      {navigationItems.map(({ icon, label }) => (
        <NavigationSideBarItem key={label} icon={icon} label={label} />
      ))}
    </div>
  );
};

const Logo = () => {
  return (
    <div className="flex flex-row gap-2 items-baseline">
      <p className="grid place-content-center w-12 h-12 rounded-xl bg-garden-sage text-garden-forest text-3xl">
        ✳
      </p>
      <p className="text-garden-cream text-sm font-bold tracking-widest">
        GREENHOUSE
      </p>
    </div>
  );
};

const NavigationSideBar = () => {
  return (
    <aside className="flex h-full w-64 shrink-0 flex-col gap-8 bg-garden-forest p-4">
      <Logo />
      <NavigationItems />
    </aside>
  );
};

export default NavigationSideBar;
