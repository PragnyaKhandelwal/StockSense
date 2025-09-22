import { useState } from 'react';
import { TrendingUp, TrendingDown, Filter } from 'lucide-react';
import { Card } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { Input } from './ui/input';

interface StockTickerProps {
  onSelectStock: (symbol: string) => void;
}

const stockData = [
  { symbol: 'RELIANCE', name: 'Reliance Industries Ltd.', price: 2847.50, change: 35.20, changePercent: 1.25 },
  { symbol: 'TCS', name: 'Tata Consultancy Services', price: 4125.30, change: -28.50, changePercent: -0.69 },
  { symbol: 'INFY', name: 'Infosys Ltd.', price: 1789.45, change: 22.15, changePercent: 1.25 },
  { symbol: 'HDFCBANK', name: 'HDFC Bank Ltd.', price: 1642.80, change: -12.30, changePercent: -0.74 },
  { symbol: 'ICICIBANK', name: 'ICICI Bank Ltd.', price: 1289.20, change: 18.50, changePercent: 1.46 },
  { symbol: 'BHARTIARTL', name: 'Bharti Airtel Ltd.', price: 1567.90, change: 25.60, changePercent: 1.66 },
  { symbol: 'ITC', name: 'ITC Ltd.', price: 485.70, change: -3.10, changePercent: -0.63 },
  { symbol: 'LT', name: 'Larsen & Toubro Ltd.', price: 3625.40, change: 42.80, changePercent: 1.20 },
];

export function StockTicker({ onSelectStock }: StockTickerProps) {
  const [filter, setFilter] = useState('');
  const [sortBy, setSortBy] = useState('none');

  const filteredStocks = stockData.filter(stock => 
    stock.symbol.toLowerCase().includes(filter.toLowerCase()) ||
    stock.name.toLowerCase().includes(filter.toLowerCase())
  );

  const sortedStocks = [...filteredStocks].sort((a, b) => {
    switch (sortBy) {
      case 'gainers':
        return b.changePercent - a.changePercent;
      case 'losers':
        return a.changePercent - b.changePercent;
      case 'volume':
        return Math.random() - 0.5; // Mock volume sort
      default:
        return 0;
    }
  });

  return (
    <Card className="p-4">
      <div className="flex items-center justify-between mb-4">
        <h2 className="text-lg font-semibold">Live Stock Ticker</h2>
        <div className="flex items-center gap-2">
          <Input
            placeholder="Filter stocks..."
            value={filter}
            onChange={(e) => setFilter(e.target.value)}
            className="w-40"
          />
          <Button
            variant={sortBy === 'gainers' ? 'default' : 'outline'}
            size="sm"
            onClick={() => setSortBy(sortBy === 'gainers' ? 'none' : 'gainers')}
          >
            Top Gainers
          </Button>
          <Button
            variant={sortBy === 'losers' ? 'default' : 'outline'}
            size="sm"
            onClick={() => setSortBy(sortBy === 'losers' ? 'none' : 'losers')}
          >
            Top Losers
          </Button>
        </div>
      </div>
      
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-3">
        {sortedStocks.map((stock) => (
          <div
            key={stock.symbol}
            onClick={() => onSelectStock(stock.symbol)}
            className="p-3 rounded-lg border border-border hover:bg-accent transition-colors cursor-pointer"
          >
            <div className="flex items-center justify-between mb-1">
              <span className="font-medium">{stock.symbol}</span>
              {stock.changePercent > 0 ? (
                <TrendingUp className="h-4 w-4 text-green-500" />
              ) : (
                <TrendingDown className="h-4 w-4 text-red-500" />
              )}
            </div>
            <div className="text-sm text-muted-foreground mb-2">{stock.name}</div>
            <div className="flex items-center justify-between">
              <span className="font-semibold">${stock.price.toFixed(2)}</span>
              <Badge
                variant={stock.changePercent > 0 ? 'default' : 'destructive'}
                className={stock.changePercent > 0 ? 'bg-green-500 hover:bg-green-600' : ''}
              >
                {stock.changePercent > 0 ? '+' : ''}{stock.changePercent.toFixed(2)}%
              </Badge>
            </div>
          </div>
        ))}
      </div>
    </Card>
  );
}