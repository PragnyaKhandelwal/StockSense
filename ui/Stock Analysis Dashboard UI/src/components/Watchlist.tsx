import { useState } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { Input } from './ui/input';
import { Heart, Plus, Trash2, TrendingUp, TrendingDown, Search } from 'lucide-react';

interface WatchlistProps {
  onSelectStock: (symbol: string) => void;
}

interface WatchlistStock {
  symbol: string;
  name: string;
  price: number;
  change: number;
  changePercent: number;
  addedDate: string;
  alertPrice?: number;
}

export function Watchlist({ onSelectStock }: WatchlistProps) {
  const [watchlistStocks, setWatchlistStocks] = useState<WatchlistStock[]>([
    {
      symbol: 'AAPL',
      name: 'Apple Inc.',
      price: 182.52,
      change: 2.45,
      changePercent: 1.36,
      addedDate: '2024-01-15',
      alertPrice: 180.00,
    },
    {
      symbol: 'GOOGL',
      name: 'Alphabet Inc.',
      price: 138.21,
      change: -1.23,
      changePercent: -0.88,
      addedDate: '2024-01-10',
    },
    {
      symbol: 'MSFT',
      name: 'Microsoft Corp.',
      price: 378.85,
      change: 4.67,
      changePercent: 1.25,
      addedDate: '2024-01-08',
      alertPrice: 375.00,
    },
  ]);

  const [newSymbol, setNewSymbol] = useState('');
  const [alertPrice, setAlertPrice] = useState('');

  const addToWatchlist = () => {
    if (!newSymbol.trim()) return;

    const newStock: WatchlistStock = {
      symbol: newSymbol.toUpperCase(),
      name: `${newSymbol.toUpperCase()} Corp.`, // Mock name
      price: Math.random() * 200 + 50, // Mock price
      change: (Math.random() - 0.5) * 10,
      changePercent: (Math.random() - 0.5) * 5,
      addedDate: new Date().toISOString().split('T')[0],
      alertPrice: alertPrice ? parseFloat(alertPrice) : undefined,
    };

    newStock.changePercent = (newStock.change / newStock.price) * 100;

    setWatchlistStocks([...watchlistStocks, newStock]);
    setNewSymbol('');
    setAlertPrice('');
  };

  const removeFromWatchlist = (symbol: string) => {
    setWatchlistStocks(watchlistStocks.filter(stock => stock.symbol !== symbol));
  };

  const totalValue = watchlistStocks.reduce((sum, stock) => sum + stock.price, 0);
  const totalChange = watchlistStocks.reduce((sum, stock) => sum + stock.change, 0);
  const totalChangePercent = (totalChange / (totalValue - totalChange)) * 100;

  return (
    <div className="p-6 space-y-6">
      <div className="flex items-center justify-between">
        <h1 className="text-2xl font-bold flex items-center gap-2">
          <Heart className="h-6 w-6 text-red-500" />
          My Watchlist
        </h1>
        <Badge variant="outline" className="text-sm">
          {watchlistStocks.length} stocks
        </Badge>
      </div>

      {/* Portfolio Summary */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <Card>
          <CardContent className="p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm text-muted-foreground">Total Value</p>
                <p className="text-2xl font-bold">${totalValue.toFixed(2)}</p>
              </div>
              <div className="text-blue-500">
                <TrendingUp className="h-8 w-8" />
              </div>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardContent className="p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm text-muted-foreground">Total Change</p>
                <p className={`text-2xl font-bold ${totalChange > 0 ? 'text-green-500' : 'text-red-500'}`}>
                  {totalChange > 0 ? '+' : ''}${totalChange.toFixed(2)}
                </p>
              </div>
              <div className={totalChange > 0 ? 'text-green-500' : 'text-red-500'}>
                {totalChange > 0 ? <TrendingUp className="h-8 w-8" /> : <TrendingDown className="h-8 w-8" />}
              </div>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardContent className="p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm text-muted-foreground">Performance</p>
                <p className={`text-2xl font-bold ${totalChangePercent > 0 ? 'text-green-500' : 'text-red-500'}`}>
                  {totalChangePercent > 0 ? '+' : ''}{totalChangePercent.toFixed(2)}%
                </p>
              </div>
              <Badge className={totalChangePercent > 0 ? 'bg-green-500 hover:bg-green-600' : 'bg-red-500 hover:bg-red-600'}>
                {totalChangePercent > 0 ? 'Gain' : 'Loss'}
              </Badge>
            </div>
          </CardContent>
        </Card>
      </div>

      {/* Add Stock Form */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Plus className="h-5 w-5" />
            Add to Watchlist
          </CardTitle>
        </CardHeader>
        <CardContent>
          <div className="flex gap-4">
            <Input
              placeholder="Stock symbol (e.g., TSLA)"
              value={newSymbol}
              onChange={(e) => setNewSymbol(e.target.value)}
              className="flex-1"
            />
            <Input
              placeholder="Alert price (optional)"
              value={alertPrice}
              onChange={(e) => setAlertPrice(e.target.value)}
              type="number"
              className="w-40"
            />
            <Button onClick={addToWatchlist}>
              Add Stock
            </Button>
          </div>
        </CardContent>
      </Card>

      {/* Watchlist Stocks */}
      <Card>
        <CardHeader>
          <CardTitle>Watched Stocks</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="space-y-4">
            {watchlistStocks.map((stock) => (
              <div
                key={stock.symbol}
                className="flex items-center justify-between p-4 border border-border rounded-lg hover:bg-accent transition-colors cursor-pointer"
                onClick={() => onSelectStock(stock.symbol)}
              >
                <div className="flex items-center gap-4">
                  <div>
                    <div className="flex items-center gap-2">
                      <h3 className="font-semibold">{stock.symbol}</h3>
                      {stock.changePercent > 0 ? (
                        <TrendingUp className="h-4 w-4 text-green-500" />
                      ) : (
                        <TrendingDown className="h-4 w-4 text-red-500" />
                      )}
                    </div>
                    <p className="text-sm text-muted-foreground">{stock.name}</p>
                    <p className="text-xs text-muted-foreground">Added: {stock.addedDate}</p>
                  </div>
                </div>

                <div className="text-right">
                  <div className="font-semibold">${stock.price.toFixed(2)}</div>
                  <div className={`text-sm ${stock.changePercent > 0 ? 'text-green-500' : 'text-red-500'}`}>
                    {stock.change > 0 ? '+' : ''}${stock.change.toFixed(2)} ({stock.changePercent.toFixed(2)}%)
                  </div>
                  {stock.alertPrice && (
                    <div className="text-xs text-muted-foreground">
                      Alert: ${stock.alertPrice.toFixed(2)}
                    </div>
                  )}
                </div>

                <Button
                  variant="ghost"
                  size="sm"
                  onClick={(e) => {
                    e.stopPropagation();
                    removeFromWatchlist(stock.symbol);
                  }}
                  className="text-red-500 hover:text-red-700 hover:bg-red-50"
                >
                  <Trash2 className="h-4 w-4" />
                </Button>
              </div>
            ))}

            {watchlistStocks.length === 0 && (
              <div className="text-center py-8 text-muted-foreground">
                <Heart className="h-12 w-12 mx-auto mb-4 opacity-50" />
                <p>No stocks in your watchlist yet.</p>
                <p className="text-sm">Add some stocks to track their performance!</p>
              </div>
            )}
          </div>
        </CardContent>
      </Card>
    </div>
  );
}