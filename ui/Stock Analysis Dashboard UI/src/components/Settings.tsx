import { useState } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Button } from './ui/button';
import { Switch } from './ui/switch';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { Separator } from './ui/separator';
import { AlgorithmMonitor } from './AlgorithmMonitor';
import { Bell, RefreshCw, Monitor, Palette, Shield, Download } from 'lucide-react';

export function Settings() {
  const [notifications, setNotifications] = useState(true);
  const [priceAlerts, setPriceAlerts] = useState(true);
  const [newsAlerts, setNewsAlerts] = useState(false);
  const [refreshInterval, setRefreshInterval] = useState('30');
  const [currency, setCurrency] = useState('USD');
  const [timezone, setTimezone] = useState('America/New_York');

  return (
    <div className="p-6 space-y-6 max-w-4xl">
      <h1 className="text-2xl font-bold">Settings</h1>

      {/* Notifications */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Bell className="h-5 w-5" />
            Notifications
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="flex items-center justify-between">
            <div>
              <Label htmlFor="notifications">Enable Notifications</Label>
              <p className="text-sm text-muted-foreground">
                Receive notifications for important updates
              </p>
            </div>
            <Switch
              id="notifications"
              checked={notifications}
              onCheckedChange={setNotifications}
            />
          </div>

          <Separator />

          <div className="flex items-center justify-between">
            <div>
              <Label htmlFor="price-alerts">Price Alerts</Label>
              <p className="text-sm text-muted-foreground">
                Get notified when stocks reach target prices
              </p>
            </div>
            <Switch
              id="price-alerts"
              checked={priceAlerts}
              onCheckedChange={setPriceAlerts}
              disabled={!notifications}
            />
          </div>

          <div className="flex items-center justify-between">
            <div>
              <Label htmlFor="news-alerts">News Alerts</Label>
              <p className="text-sm text-muted-foreground">
                Receive notifications for breaking financial news
              </p>
            </div>
            <Switch
              id="news-alerts"
              checked={newsAlerts}
              onCheckedChange={setNewsAlerts}
              disabled={!notifications}
            />
          </div>
        </CardContent>
      </Card>

      {/* Data & Refresh */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <RefreshCw className="h-5 w-5" />
            Data & Refresh Settings
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-2">
            <Label>Auto-refresh Interval</Label>
            <Select value={refreshInterval} onValueChange={setRefreshInterval}>
              <SelectTrigger className="w-full">
                <SelectValue placeholder="Select refresh interval" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="5">5 seconds</SelectItem>
                <SelectItem value="15">15 seconds</SelectItem>
                <SelectItem value="30">30 seconds</SelectItem>
                <SelectItem value="60">1 minute</SelectItem>
                <SelectItem value="300">5 minutes</SelectItem>
                <SelectItem value="0">Manual only</SelectItem>
              </SelectContent>
            </Select>
            <p className="text-sm text-muted-foreground">
              How often to refresh stock prices and data
            </p>
          </div>

          <Separator />

          <div className="space-y-2">
            <Label>Default Currency</Label>
            <Select value={currency} onValueChange={setCurrency}>
              <SelectTrigger className="w-full">
                <SelectValue placeholder="Select currency" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="USD">USD - US Dollar</SelectItem>
                <SelectItem value="EUR">EUR - Euro</SelectItem>
                <SelectItem value="GBP">GBP - British Pound</SelectItem>
                <SelectItem value="JPY">JPY - Japanese Yen</SelectItem>
                <SelectItem value="CAD">CAD - Canadian Dollar</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div className="space-y-2">
            <Label>Timezone</Label>
            <Select value={timezone} onValueChange={setTimezone}>
              <SelectTrigger className="w-full">
                <SelectValue placeholder="Select timezone" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="America/New_York">Eastern Time (ET)</SelectItem>
                <SelectItem value="America/Chicago">Central Time (CT)</SelectItem>
                <SelectItem value="America/Denver">Mountain Time (MT)</SelectItem>
                <SelectItem value="America/Los_Angeles">Pacific Time (PT)</SelectItem>
                <SelectItem value="Europe/London">London (GMT)</SelectItem>
                <SelectItem value="Europe/Frankfurt">Frankfurt (CET)</SelectItem>
                <SelectItem value="Asia/Tokyo">Tokyo (JST)</SelectItem>
              </SelectContent>
            </Select>
          </div>
        </CardContent>
      </Card>

      {/* Display */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Monitor className="h-5 w-5" />
            Display Preferences
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="space-y-2">
            <Label>Chart Type Default</Label>
            <Select defaultValue="line">
              <SelectTrigger className="w-full">
                <SelectValue placeholder="Select default chart type" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="line">Line Chart</SelectItem>
                <SelectItem value="area">Area Chart</SelectItem>
                <SelectItem value="candlestick">Candlestick</SelectItem>
                <SelectItem value="volume">Volume</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div className="space-y-2">
            <Label>Default Time Range</Label>
            <Select defaultValue="1M">
              <SelectTrigger className="w-full">
                <SelectValue placeholder="Select default time range" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="1D">1 Day</SelectItem>
                <SelectItem value="1W">1 Week</SelectItem>
                <SelectItem value="1M">1 Month</SelectItem>
                <SelectItem value="1Y">1 Year</SelectItem>
                <SelectItem value="5Y">5 Years</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div className="flex items-center justify-between">
            <div>
              <Label>Compact Mode</Label>
              <p className="text-sm text-muted-foreground">
                Show more data in less space
              </p>
            </div>
            <Switch defaultChecked={false} />
          </div>
        </CardContent>
      </Card>

      {/* Privacy & Security */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Shield className="h-5 w-5" />
            Privacy & Security
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="flex items-center justify-between">
            <div>
              <Label>Analytics Sharing</Label>
              <p className="text-sm text-muted-foreground">
                Help improve the app by sharing anonymous usage data
              </p>
            </div>
            <Switch defaultChecked={true} />
          </div>

          <div className="flex items-center justify-between">
            <div>
              <Label>Crash Reports</Label>
              <p className="text-sm text-muted-foreground">
                Send crash reports to help fix issues
              </p>
            </div>
            <Switch defaultChecked={true} />
          </div>
        </CardContent>
      </Card>

      {/* Data Export */}
      <Card>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Download className="h-5 w-5" />
            Data Export
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <Button variant="outline">
              Export Watchlist
            </Button>
            <Button variant="outline">
              Export Trading History
            </Button>
            <Button variant="outline">
              Export Settings
            </Button>
            <Button variant="outline">
              Export All Data
            </Button>
          </div>
        </CardContent>
      </Card>

      {/* Algorithm Performance Monitor */}
      <AlgorithmMonitor />

      {/* Actions */}
      <div className="flex gap-4 pt-4">
        <Button>Save Changes</Button>
        <Button variant="outline">Reset to Defaults</Button>
      </div>
    </div>
  );
}