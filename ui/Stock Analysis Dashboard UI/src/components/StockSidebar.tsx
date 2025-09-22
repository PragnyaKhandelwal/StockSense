import { BarChart3, TrendingUp, Heart, Settings, Home } from 'lucide-react';
import { cn } from './ui/utils';
import { Button } from './ui/button';

interface StockSidebarProps {
  currentView: string;
  setCurrentView: (view: string) => void;
}

const navigationItems = [
  {
    id: 'dashboard',
    label: 'Dashboard',
    icon: Home,
  },
  {
    id: 'predictions',
    label: 'Analysis',
    icon: TrendingUp,
  },
  {
    id: 'watchlist',
    label: 'My Watchlist',
    icon: Heart,
  },
  {
    id: 'settings',
    label: 'Settings',
    icon: Settings,
  },
];

export function StockSidebar({ currentView, setCurrentView }: StockSidebarProps) {
  return (
    <aside className="w-64 bg-sidebar border-r border-sidebar-border p-4">
      <nav className="space-y-2">
        {navigationItems.map((item) => {
          const Icon = item.icon;
          return (
            <Button
              key={item.id}
              variant={currentView === item.id ? "default" : "ghost"}
              className={cn(
                "w-full justify-start gap-3",
                currentView === item.id && "bg-sidebar-primary text-sidebar-primary-foreground"
              )}
              onClick={() => setCurrentView(item.id)}
            >
              <Icon className="h-5 w-5" />
              {item.label}
            </Button>
          );
        })}
      </nav>

      <div className="mt-8 p-4 bg-sidebar-accent rounded-lg">
        <h3 className="font-medium mb-2">Market Status</h3>
        <div className="space-y-2 text-sm">
          <div className="flex justify-between">
            <span>NSE</span>
            <span className="text-green-500">● Open</span>
          </div>
          <div className="flex justify-between">
            <span>BSE</span>
            <span className="text-green-500">● Open</span>
          </div>
          <div className="flex justify-between">
            <span>MCX</span>
            <span className="text-yellow-500">● Pre-Open</span>
          </div>
        </div>
      </div>
    </aside>
  );
}