import { Search, User, Sun, Moon } from 'lucide-react';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Switch } from './ui/switch';
import { Avatar, AvatarFallback, AvatarImage } from './ui/avatar';
import { StockSenseLogo } from './StockSenseLogo';
import { NetworkStatus } from './NetworkStatus';
import { useState } from 'react';

interface HeaderProps {
  darkMode: boolean;
  setDarkMode: (dark: boolean) => void;
  onSelectStock: (symbol: string) => void;
}

const popularStocks = ['RELIANCE', 'TCS', 'INFY', 'HDFCBANK', 'ICICIBANK', 'BHARTIARTL', 'ITC'];

export function Header({ darkMode, setDarkMode, onSelectStock }: HeaderProps) {
  const [searchQuery, setSearchQuery] = useState('');
  const [showSuggestions, setShowSuggestions] = useState(false);

  const filteredStocks = popularStocks.filter(stock => 
    stock.toLowerCase().includes(searchQuery.toLowerCase())
  );

  const handleStockSelect = (symbol: string) => {
    setSearchQuery('');
    setShowSuggestions(false);
    onSelectStock(symbol);
  };

  return (
    <header className="border-b border-border bg-card px-6 py-4 flex items-center justify-between">
      <div className="flex items-center gap-4">
        <StockSenseLogo size="md" />
      </div>

      <div className="flex-1 max-w-md mx-8 relative">
        <div className="relative">
          <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-4 w-4 text-muted-foreground" />
          <Input
            placeholder="Search stocks (e.g., RELIANCE, TCS)..."
            value={searchQuery}
            onChange={(e) => {
              setSearchQuery(e.target.value);
              setShowSuggestions(e.target.value.length > 0);
            }}
            onFocus={() => setShowSuggestions(searchQuery.length > 0)}
            onBlur={() => setTimeout(() => setShowSuggestions(false), 200)}
            className="pl-10"
          />
        </div>
        {showSuggestions && filteredStocks.length > 0 && (
          <div className="absolute top-full left-0 right-0 mt-1 bg-popover border border-border rounded-md shadow-lg z-50">
            {filteredStocks.map((stock) => (
              <button
                key={stock}
                onClick={() => handleStockSelect(stock)}
                className="w-full px-4 py-2 text-left hover:bg-accent hover:text-accent-foreground transition-colors"
              >
                {stock}
              </button>
            ))}
          </div>
        )}
      </div>

      <div className="flex items-center gap-4">
        <NetworkStatus />
        <div className="flex items-center gap-3 px-3 py-1.5 rounded-lg border border-border bg-card">
          <Moon className={`h-4 w-4 transition-colors ${!darkMode ? 'text-muted-foreground' : 'text-primary'}`} />
          <Switch
            checked={darkMode}
            onCheckedChange={setDarkMode}
          />
          <Sun className={`h-4 w-4 transition-colors ${darkMode ? 'text-muted-foreground' : 'text-primary'}`} />
        </div>
        <Avatar className="border-2 border-border">
          <AvatarImage src="/placeholder-avatar.jpg" alt="User" />
          <AvatarFallback className="bg-primary/10">
            <User className="h-4 w-4 text-primary" />
          </AvatarFallback>
        </Avatar>
      </div>
    </header>
  );
}