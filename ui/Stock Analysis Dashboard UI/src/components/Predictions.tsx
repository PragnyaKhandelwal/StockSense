import { useState } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, ReferenceLine } from 'recharts';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { TrendingUp, TrendingDown, Brain, Target } from 'lucide-react';

interface AnalysisProps {
  selectedStock: string;
}

// Generate mock prediction data
const generatePredictionData = (days: number) => {
  const data = [];
  let basePrice = 2847.50; // Current price for Indian stock
  const currentDate = new Date();
  
  // Historical data (last 30 days)
  for (let i = -30; i < 0; i++) {
    const change = (Math.random() - 0.5) * 8;
    basePrice += change;
    data.push({
      date: new Date(currentDate.getTime() + i * 24 * 60 * 60 * 1000).toLocaleDateString(),
      actual: Math.max(basePrice, 10),
      predicted: null,
      confidence: null,
      type: 'historical'
    });
  }
  
  // Current price point
  data.push({
    date: currentDate.toLocaleDateString(),
    actual: 2847.50,
    predicted: 2847.50,
    confidence: 100,
    type: 'current'
  });
  
  // Future predictions
  basePrice = 2847.50;
  for (let i = 1; i <= days; i++) {
    const trend = 0.02; // Slight upward trend
    const volatility = (Math.random() - 0.5) * 6;
    basePrice += trend + volatility;
    const confidence = Math.max(100 - (i * 2), 20); // Decreasing confidence over time
    
    data.push({
      date: new Date(currentDate.getTime() + i * 24 * 60 * 60 * 1000).toLocaleDateString(),
      actual: null,
      predicted: Math.max(basePrice, 10),
      confidence: confidence,
      type: 'predicted'
    });
  }
  
  return data;
};

const analysisModels = [
  { 
    id: 'movingavg', 
    name: 'Moving Average Analysis', 
    accuracy: '78%', 
    icon: TrendingUp,
    algorithm: 'Sliding Window',
    complexity: 'O(n)'
  },
  { 
    id: 'fibonacci', 
    name: 'Fibonacci Retracement', 
    accuracy: '74%', 
    icon: Target,
    algorithm: 'Dynamic Programming',
    complexity: 'O(1)'
  },
  { 
    id: 'technical', 
    name: 'Multi-Indicator Analysis', 
    accuracy: '82%', 
    icon: Brain,
    algorithm: 'Ensemble Methods',
    complexity: 'O(n log n)'
  },
];

export function Predictions({ selectedStock }: AnalysisProps) {
  const [timeRange, setTimeRange] = useState(30);
  const [selectedModel, setSelectedModel] = useState('technical');
  
  const data = generatePredictionData(timeRange);
  const currentPrice = 2847.50;
  const predictedPrice = data[data.length - 1]?.predicted || 0;
  const change = predictedPrice - currentPrice;
  const changePercent = (change / currentPrice) * 100;

  const CustomTooltip = ({ active, payload, label }: any) => {
    if (active && payload && payload.length) {
      return (
        <div className="bg-popover border border-border rounded-md p-3 shadow-lg">
          <p className="font-medium">{label}</p>
          {payload.map((entry: any, index: number) => (
            <div key={index}>
              {entry.dataKey === 'actual' && entry.value && (
                <p className="text-blue-500">Actual: ${entry.value.toFixed(2)}</p>
              )}
              {entry.dataKey === 'predicted' && entry.value && (
                <div>
                  <p className="text-orange-500">Projected: ₹{entry.value.toFixed(2)}</p>
                  {entry.payload.confidence && (
                    <p className="text-xs text-muted-foreground">
                      Confidence: {entry.payload.confidence.toFixed(0)}%
                    </p>
                  )}
                </div>
              )}
            </div>
          ))}
        </div>
      );
    }
    return null;
  };

  return (
    <div className="p-6 space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-2xl font-bold">Algorithmic Analysis for {selectedStock}</h1>
          <p className="text-muted-foreground">Advanced data structures and algorithms for stock prediction</p>
        </div>
        <div className="flex gap-2">
          {[7, 15, 30, 60].map((days) => (
            <Button
              key={days}
              variant={timeRange === days ? 'default' : 'outline'}
              size="sm"
              onClick={() => setTimeRange(days)}
            >
              {days}D
            </Button>
          ))}
        </div>
      </div>

      {/* Prediction Summary */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <Card>
          <CardContent className="p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm text-muted-foreground">Current Price</p>
                <p className="text-2xl font-bold">₹{currentPrice.toFixed(2)}</p>
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
                <p className="text-sm text-muted-foreground">Projected Price ({timeRange}D)</p>
                <p className="text-2xl font-bold">₹{predictedPrice.toFixed(2)}</p>
              </div>
              <div className={change > 0 ? 'text-green-500' : 'text-red-500'}>
                {change > 0 ? <TrendingUp className="h-8 w-8" /> : <TrendingDown className="h-8 w-8" />}
              </div>
            </div>
          </CardContent>
        </Card>

        <Card>
          <CardContent className="p-6">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm text-muted-foreground">Expected Change</p>
                <p className={`text-2xl font-bold ${change > 0 ? 'text-green-500' : 'text-red-500'}`}>
                  {change > 0 ? '+' : ''}{changePercent.toFixed(2)}%
                </p>
              </div>
              <Badge className={change > 0 ? 'bg-green-500 hover:bg-green-600' : 'bg-red-500 hover:bg-red-600'}>
                {change > 0 ? 'Bullish' : 'Bearish'}
              </Badge>
            </div>
          </CardContent>
        </Card>
      </div>

      {/* Model Selection */}
      <Card>
        <CardHeader>
          <CardTitle>Analysis Methods</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            {analysisModels.map((model) => {
              const Icon = model.icon;
              return (
                <div
                  key={model.id}
                  onClick={() => setSelectedModel(model.id)}
                  className={`p-4 rounded-lg border cursor-pointer transition-colors ${
                    selectedModel === model.id 
                      ? 'border-primary bg-primary/5' 
                      : 'border-border hover:bg-accent'
                  }`}
                >
                  <div className="flex items-center gap-3">
                    <Icon className="h-5 w-5" />
                    <div className="flex-1">
                      <h3 className="font-medium">{model.name}</h3>
                      <p className="text-sm text-muted-foreground">{model.algorithm}</p>
                      <div className="flex items-center gap-3 mt-1">
                        <span className="text-xs font-mono text-green-600">{model.complexity}</span>
                        <span className="text-xs text-muted-foreground">• {model.accuracy}</span>
                      </div>
                    </div>
                  </div>
                </div>
              );
            })}
          </div>
        </CardContent>
      </Card>

      {/* Prediction Chart */}
      <Card>
        <CardHeader>
          <CardTitle>Price Prediction Visualization</CardTitle>
        </CardHeader>
        <CardContent>
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
              <ReferenceLine x={new Date().toLocaleDateString()} stroke="hsl(var(--primary))" strokeDasharray="2 2" />
              <Line 
                type="monotone" 
                dataKey="actual" 
                stroke="#3b82f6"
                strokeWidth={2}
                dot={false}
                connectNulls={false}
              />
              <Line 
                type="monotone" 
                dataKey="predicted" 
                stroke="#f97316"
                strokeWidth={2}
                strokeDasharray="5 5"
                dot={false}
                connectNulls={false}
              />
            </LineChart>
          </ResponsiveContainer>
          <div className="flex items-center justify-center gap-6 mt-4 text-sm">
            <div className="flex items-center gap-2">
              <div className="w-4 h-0.5 bg-blue-500"></div>
              <span>Actual Price</span>
            </div>
            <div className="flex items-center gap-2">
              <div className="w-4 h-0.5 bg-orange-500" style={{ backgroundImage: 'repeating-linear-gradient(to right, #f97316 0, #f97316 5px, transparent 5px, transparent 10px)' }}></div>
              <span>Predicted Price</span>
            </div>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}