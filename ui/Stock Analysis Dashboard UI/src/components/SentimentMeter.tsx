import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Progress } from './ui/progress';
import { Badge } from './ui/badge';

interface SentimentMeterProps {
  selectedStock: string;
}

export function SentimentMeter({ selectedStock }: SentimentMeterProps) {
  // Mock sentiment data
  const sentiment = {
    score: 72, // 0-100
    label: 'Bullish',
    analyst_rating: 'Buy',
    social_sentiment: 68,
    news_sentiment: 75,
    technical_sentiment: 70,
  };

  const getSentimentColor = (score: number) => {
    if (score >= 70) return 'text-green-500';
    if (score >= 40) return 'text-yellow-500';
    return 'text-red-500';
  };

  const getSentimentBadgeColor = (score: number) => {
    if (score >= 70) return 'bg-green-500 hover:bg-green-600';
    if (score >= 40) return 'bg-yellow-500 hover:bg-yellow-600';
    return 'bg-red-500 hover:bg-red-600';
  };

  return (
    <Card>
      <CardHeader>
        <CardTitle>Sentiment Analysis</CardTitle>
      </CardHeader>
      <CardContent className="space-y-4">
        {/* Overall Sentiment */}
        <div className="text-center">
          <div className={`text-3xl font-bold ${getSentimentColor(sentiment.score)}`}>
            {sentiment.score}%
          </div>
          <Badge className={getSentimentBadgeColor(sentiment.score)}>
            {sentiment.label}
          </Badge>
        </div>

        {/* Sentiment Breakdown */}
        <div className="space-y-3">
          <div>
            <div className="flex justify-between text-sm mb-1">
              <span>Social Media</span>
              <span className={getSentimentColor(sentiment.social_sentiment)}>
                {sentiment.social_sentiment}%
              </span>
            </div>
            <Progress value={sentiment.social_sentiment} className="h-2" />
          </div>

          <div>
            <div className="flex justify-between text-sm mb-1">
              <span>News Sentiment</span>
              <span className={getSentimentColor(sentiment.news_sentiment)}>
                {sentiment.news_sentiment}%
              </span>
            </div>
            <Progress value={sentiment.news_sentiment} className="h-2" />
          </div>

          <div>
            <div className="flex justify-between text-sm mb-1">
              <span>Technical Analysis</span>
              <span className={getSentimentColor(sentiment.technical_sentiment)}>
                {sentiment.technical_sentiment}%
              </span>
            </div>
            <Progress value={sentiment.technical_sentiment} className="h-2" />
          </div>
        </div>

        {/* Analyst Rating */}
        <div className="pt-2 border-t border-border">
          <div className="flex justify-between items-center">
            <span className="text-sm text-muted-foreground">Analyst Rating</span>
            <Badge className="bg-blue-500 hover:bg-blue-600">
              {sentiment.analyst_rating}
            </Badge>
          </div>
        </div>
      </CardContent>
    </Card>
  );
}