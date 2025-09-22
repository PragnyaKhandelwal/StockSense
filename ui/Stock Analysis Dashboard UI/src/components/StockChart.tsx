import { useState } from 'react';
import { LineChart, Line, AreaChart, Area, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, BarChart, Bar } from 'recharts';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { TrendingUp, TrendingDown } from 'lucide-react';
import { Tabs, TabsContent, TabsList, TabsTrigger } from './ui/tabs';

interface StockChartProps {
  selectedStock: string;
}

// Mock data generator for Indian stocks
const generateMockData = (days: number) => {
  const data = [];
  let basePrice = 1500 + Math.random() * 2000; // Higher base price for Indian stocks
  
  for (let i = 0; i < days; i++) {
    const change = (Math.random() - 0.5) * 10;
    basePrice += change;
    const volume = Math.floor(Math.random() * 10000000) + 1000000;
    
    data.push({
      date: new Date(Date.now() - (days - i) * 24 * 60 * 60 * 1000).toLocaleDateString(),
      price: Math.max(basePrice, 10),
      volume: volume,
      high: Math.max(basePrice + Math.random() * 5, basePrice),
      low: Math.max(basePrice - Math.random() * 5, 10),
      open: Math.max(basePrice + (Math.random() - 0.5) * 3, 10),
      close: Math.max(basePrice, 10),
    });
  }
  
  return data;
};

const timeRanges = [
  { label: '1D', days: 1 },
  { label: '1W', days: 7 },
  { label: '1M', days: 30 },
  { label: '1Y', days: 365 },
  { label: '5Y', days: 1825 },
];

export function StockChart({ selectedStock }: StockChartProps) {
  const [timeRange, setTimeRange] = useState('1M');
  const [chartType, setChartType] = useState('line');
  
  const selectedRange = timeRanges.find(range => range.label === timeRange) || timeRanges[2];
  const data = generateMockData(selectedRange.days);
  const currentPrice = data[data.length - 1]?.price || 0;
  const previousPrice = data[data.length - 2]?.price || currentPrice;
  const change = currentPrice - previousPrice;
  const changePercent = ((change / previousPrice) * 100);

  const CustomTooltip = ({ active, payload, label }: any) => {
    if (active && payload && payload.length) {
      return (
        <div className="bg-popover border border-border rounded-md p-3 shadow-lg">
          <p className="font-medium">{label}</p>
          <p className="text-primary">
            Price: ₹{payload[0].value.toFixed(2)}
          </p>
          {payload[1] && (
            <p className="text-muted-foreground">
              Volume: {payload[1].value.toLocaleString()}
            </p>
          )}
        </div>
      );
    }
    return null;
  };

  return (
    <Card>
      <CardHeader>
        <div className="flex items-center justify-between">
          <div>
            <CardTitle className="flex items-center gap-2">
              {selectedStock}
              {change > 0 ? (
                <TrendingUp className="h-5 w-5 text-green-500" />
              ) : (
                <TrendingDown className="h-5 w-5 text-red-500" />
              )}
            </CardTitle>
            <div className="flex items-center gap-2 mt-1">
              <span className="text-2xl font-bold">₹{currentPrice.toFixed(2)}</span>
              <Badge
                variant={change > 0 ? 'default' : 'destructive'}
                className={change > 0 ? 'bg-green-500 hover:bg-green-600' : ''}
              >
                {change > 0 ? '+' : ''}{change.toFixed(2)} ({changePercent.toFixed(2)}%)
              </Badge>
            </div>
          </div>
          
          <div className="flex gap-2">
            {timeRanges.map((range) => (
              <Button
                key={range.label}
                variant={timeRange === range.label ? 'default' : 'outline'}
                size="sm"
                onClick={() => setTimeRange(range.label)}
              >
                {range.label}
              </Button>
            ))}
          </div>
        </div>
      </CardHeader>
      
      <CardContent>
        <Tabs value={chartType} onValueChange={setChartType} className="mb-4">
          <TabsList>
            <TabsTrigger value="line">Line</TabsTrigger>
            <TabsTrigger value="area">Area</TabsTrigger>
            <TabsTrigger value="volume">Volume</TabsTrigger>
          </TabsList>
          
          <TabsContent value="line" className="mt-4">
            <ResponsiveContainer width="100%" height={400}>
              <LineChart data={data}>
                <CartesianGrid strokeDasharray="3 3" stroke="hsl(var(--border))" />
                <XAxis 
                  dataKey="date" 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                />
                <YAxis 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                  domain={['dataMin - 5', 'dataMax + 5']}
                />
                <Tooltip content={<CustomTooltip />} />
                <Line 
                  type="monotone" 
                  dataKey="price" 
                  stroke={change > 0 ? '#22c55e' : '#ef4444'}
                  strokeWidth={2}
                  dot={false}
                  activeDot={{ r: 4, fill: change > 0 ? '#22c55e' : '#ef4444' }}
                />
              </LineChart>
            </ResponsiveContainer>
          </TabsContent>
          
          <TabsContent value="area" className="mt-4">
            <ResponsiveContainer width="100%" height={400}>
              <AreaChart data={data}>
                <CartesianGrid strokeDasharray="3 3" stroke="hsl(var(--border))" />
                <XAxis 
                  dataKey="date" 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                />
                <YAxis 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                  domain={['dataMin - 5', 'dataMax + 5']}
                />
                <Tooltip content={<CustomTooltip />} />
                <Area 
                  type="monotone" 
                  dataKey="price" 
                  stroke={change > 0 ? '#22c55e' : '#ef4444'}
                  fill={change > 0 ? '#22c55e' : '#ef4444'}
                  fillOpacity={0.2}
                  strokeWidth={2}
                />
              </AreaChart>
            </ResponsiveContainer>
          </TabsContent>
          
          <TabsContent value="volume" className="mt-4">
            <ResponsiveContainer width="100%" height={400}>
              <BarChart data={data}>
                <CartesianGrid strokeDasharray="3 3" stroke="hsl(var(--border))" />
                <XAxis 
                  dataKey="date" 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                />
                <YAxis 
                  stroke="hsl(var(--muted-foreground))"
                  fontSize={12}
                />
                <Tooltip content={<CustomTooltip />} />
                <Bar 
                  dataKey="volume" 
                  fill="hsl(var(--primary))"
                  opacity={0.7}
                />
              </BarChart>
            </ResponsiveContainer>
          </TabsContent>
        </Tabs>
      </CardContent>
    </Card>
  );
}