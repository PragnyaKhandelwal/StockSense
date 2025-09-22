import { StockTicker } from './StockTicker';
import { StockChart } from './StockChart';
import { StockStats } from './StockStats';
import { DataStructuresAnalyzer } from './DataStructuresAnalyzer';
import { NewsPanel } from './NewsPanel';
import { QuickStats } from './QuickStats';

interface DashboardProps {
  selectedStock: string;
  onSelectStock: (symbol: string) => void;
}

export function Dashboard({ selectedStock, onSelectStock }: DashboardProps) {
  return (
    <div className="p-6 space-y-6">
      {/* Top Row - Stock Ticker */}
      <StockTicker onSelectStock={onSelectStock} />
      
      {/* Main Content Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-4 gap-6">
        {/* Left Column - Chart and Stats */}
        <div className="lg:col-span-3 space-y-6">
          <StockChart selectedStock={selectedStock} />
          <StockStats selectedStock={selectedStock} />
        </div>
        
        {/* Right Column - Widgets */}
        <div className="space-y-6">
          <QuickStats selectedStock={selectedStock} />
          <DataStructuresAnalyzer selectedStock={selectedStock} />
          <NewsPanel selectedStock={selectedStock} />
        </div>
      </div>
    </div>
  );
}