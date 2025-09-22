import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Badge } from './ui/badge';
import { Progress } from './ui/progress';

interface StockStatsProps {
  selectedStock: string;
}

export function StockStats({ selectedStock }: StockStatsProps) {
  // Mock data for the selected stock (Indian market)
  const stats = {
    marketCap: '₹18.2L Cr',
    peRatio: '24.85',
    eps: '₹114.50',
    dividendYield: '0.35%',
    weekHigh52: '₹3,024.90',
    weekLow52: '₹2,220.30',
    avgVolume: '1.2Cr',
    beta: '0.95',
    revenue: '₹7.9L Cr',
    netIncome: '₹75,946 Cr',
    grossMargin: '42.8%',
    operatingMargin: '27.5%',
  };

  const currentPrice = 2847.50; // Mock current price for Indian stock
  const high52 = 3024.90;
  const low52 = 2220.30;
  const pricePosition = ((currentPrice - low52) / (high52 - low52)) * 100;

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
      {/* Key Metrics */}
      <Card>
        <CardHeader>
          <CardTitle>Key Metrics</CardTitle>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="flex justify-between">
            <span className="text-muted-foreground">Market Cap</span>
            <span className="font-semibold">{stats.marketCap}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">P/E Ratio</span>
            <span className="font-semibold">{stats.peRatio}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">EPS</span>
            <span className="font-semibold">{stats.eps}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">Dividend Yield</span>
            <span className="font-semibold">{stats.dividendYield}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">Beta</span>
            <Badge variant="outline">{stats.beta}</Badge>
          </div>
        </CardContent>
      </Card>

      {/* 52-Week Range */}
      <Card>
        <CardHeader>
          <CardTitle>52-Week Range</CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="flex justify-between text-sm">
            <span className="text-muted-foreground">Low: {stats.weekLow52}</span>
            <span className="text-muted-foreground">High: {stats.weekHigh52}</span>
          </div>
          <div className="space-y-2">
            <Progress value={pricePosition} className="h-2" />
            <div className="text-center">
              <span className="text-sm font-medium">Current: ₹{currentPrice}</span>
              <div className="text-xs text-muted-foreground">
                {pricePosition.toFixed(1)}% of 52-week range
              </div>
            </div>
          </div>
          <div className="flex justify-between text-sm">
            <span className="text-muted-foreground">Avg Volume</span>
            <span className="font-semibold">{stats.avgVolume}</span>
          </div>
        </CardContent>
      </Card>

      {/* Financial Performance */}
      <Card>
        <CardHeader>
          <CardTitle>Financial Performance</CardTitle>
        </CardHeader>
        <CardContent className="space-y-3">
          <div className="flex justify-between">
            <span className="text-muted-foreground">Revenue (TTM)</span>
            <span className="font-semibold">{stats.revenue}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">Net Income</span>
            <span className="font-semibold">{stats.netIncome}</span>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">Gross Margin</span>
            <Badge className="bg-green-500 hover:bg-green-600">{stats.grossMargin}</Badge>
          </div>
          <div className="flex justify-between">
            <span className="text-muted-foreground">Operating Margin</span>
            <Badge className="bg-blue-500 hover:bg-blue-600">{stats.operatingMargin}</Badge>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}