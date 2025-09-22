import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { TrendingUp, TrendingDown, DollarSign, BarChart } from 'lucide-react';

interface QuickStatsProps {
  selectedStock: string;
}

export function QuickStats({ selectedStock }: QuickStatsProps) {
  const stats = [
    {
      title: 'Market Cap',
      value: '$2.89T',
      change: '+2.1%',
      positive: true,
      icon: DollarSign,
    },
    {
      title: '52W High',
      value: '$198.23',
      change: '-7.9%',
      positive: false,
      icon: TrendingUp,
    },
    {
      title: '52W Low',
      value: '$124.17',
      change: '+47.0%',
      positive: true,
      icon: TrendingDown,
    },
    {
      title: 'P/E Ratio',
      value: '28.45',
      change: '-0.3',
      positive: false,
      icon: BarChart,
    },
  ];

  return (
    <div className="space-y-4">
      <h3 className="font-semibold">Quick Stats</h3>
      {stats.map((stat, index) => {
        const Icon = stat.icon;
        return (
          <Card key={index} className="p-4">
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-2">
                <Icon className="h-4 w-4 text-muted-foreground" />
                <span className="text-sm text-muted-foreground">{stat.title}</span>
              </div>
              <div className="text-right">
                <div className="font-semibold">{stat.value}</div>
                <div className={`text-xs ${stat.positive ? 'text-green-500' : 'text-red-500'}`}>
                  {stat.change}
                </div>
              </div>
            </div>
          </Card>
        );
      })}
    </div>
  );
}