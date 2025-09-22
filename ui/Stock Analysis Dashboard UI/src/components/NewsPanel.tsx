import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Badge } from './ui/badge';
import { ExternalLink, Clock } from 'lucide-react';

interface NewsPanelProps {
  selectedStock: string;
}

export function NewsPanel({ selectedStock }: NewsPanelProps) {
  // Mock news data
  const news = [
    {
      title: `${selectedStock} Reports Strong Q4 Earnings`,
      source: 'Financial Times',
      time: '2 hours ago',
      sentiment: 'positive',
      summary: 'Company beats analyst expectations with revenue growth of 12%.',
    },
    {
      title: 'Market Analysis: Tech Stocks Outlook',
      source: 'Bloomberg',
      time: '4 hours ago',
      sentiment: 'neutral',
      summary: 'Industry experts discuss the future of technology investments.',
    },
    {
      title: 'Regulatory Concerns Impact Stock Price',
      source: 'Reuters',
      time: '1 day ago',
      sentiment: 'negative',
      summary: 'New regulations may affect company operations in key markets.',
    },
  ];

  const getSentimentColor = (sentiment: string) => {
    switch (sentiment) {
      case 'positive':
        return 'bg-green-500 hover:bg-green-600';
      case 'negative':
        return 'bg-red-500 hover:bg-red-600';
      default:
        return 'bg-gray-500 hover:bg-gray-600';
    }
  };

  return (
    <Card>
      <CardHeader>
        <CardTitle className="flex items-center justify-between">
          News & Insights
          <ExternalLink className="h-4 w-4" />
        </CardTitle>
      </CardHeader>
      <CardContent className="space-y-4">
        {news.map((article, index) => (
          <div key={index} className="space-y-2 pb-3 border-b border-border last:border-b-0">
            <div className="flex items-start justify-between gap-2">
              <h4 className="font-medium text-sm leading-tight">{article.title}</h4>
              <Badge className={`text-xs ${getSentimentColor(article.sentiment)}`}>
                {article.sentiment}
              </Badge>
            </div>
            
            <p className="text-xs text-muted-foreground">{article.summary}</p>
            
            <div className="flex items-center justify-between text-xs text-muted-foreground">
              <span>{article.source}</span>
              <div className="flex items-center gap-1">
                <Clock className="h-3 w-3" />
                <span>{article.time}</span>
              </div>
            </div>
          </div>
        ))}
        
        <button className="w-full text-sm text-primary hover:underline pt-2">
          View all news →
        </button>
      </CardContent>
    </Card>
  );
}