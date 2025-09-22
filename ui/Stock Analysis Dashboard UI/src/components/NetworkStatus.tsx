import { useState, useEffect } from 'react';
import { Wifi, WifiOff, Globe, Server, Clock } from 'lucide-react';
import { Badge } from './ui/badge';
import { Card, CardContent } from './ui/card';
import { Tooltip, TooltipContent, TooltipTrigger, TooltipProvider } from './ui/tooltip';

export function NetworkStatus() {
  const [isOnline, setIsOnline] = useState(navigator.onLine);
  const [connectionType, setConnectionType] = useState('unknown');
  const [latency, setLatency] = useState(0);
  const [lastUpdate, setLastUpdate] = useState(new Date());

  useEffect(() => {
    const handleOnline = () => {
      setIsOnline(true);
      checkConnection();
    };

    const handleOffline = () => {
      setIsOnline(false);
      setLatency(0);
    };

    const checkConnection = async () => {
      try {
        const start = Date.now();
        // Use a lightweight endpoint or image for ping test
        await fetch('/favicon.ico', { 
          method: 'HEAD', 
          mode: 'no-cors',
          cache: 'no-cache'
        });
        const end = Date.now();
        setLatency(end - start);
        setLastUpdate(new Date());
      } catch (error) {
        setLatency(0);
      }
    };

    // Get connection type if available
    const updateConnectionType = () => {
      const connection = (navigator as any).connection || 
                        (navigator as any).mozConnection || 
                        (navigator as any).webkitConnection;
      
      if (connection) {
        setConnectionType(connection.effectiveType || connection.type || 'unknown');
      }
    };

    window.addEventListener('online', handleOnline);
    window.addEventListener('offline', handleOffline);

    // Initial checks
    if (isOnline) {
      checkConnection();
      updateConnectionType();
    }

    // Periodic connection check every 30 seconds
    const interval = setInterval(() => {
      if (isOnline) {
        checkConnection();
      }
    }, 30000);

    return () => {
      window.removeEventListener('online', handleOnline);
      window.removeEventListener('offline', handleOffline);
      clearInterval(interval);
    };
  }, [isOnline]);

  const getConnectionQuality = (latency: number) => {
    if (latency === 0) return { label: 'Offline', color: 'bg-red-500', textColor: 'text-red-500' };
    if (latency < 50) return { label: 'Excellent', color: 'bg-green-500', textColor: 'text-green-500' };
    if (latency < 100) return { label: 'Good', color: 'bg-yellow-500', textColor: 'text-yellow-500' };
    if (latency < 200) return { label: 'Fair', color: 'bg-orange-500', textColor: 'text-orange-500' };
    return { label: 'Poor', color: 'bg-red-500', textColor: 'text-red-500' };
  };

  const quality = getConnectionQuality(latency);

  return (
    <TooltipProvider>
      <Tooltip>
        <TooltipTrigger asChild>
          <div className="flex items-center gap-2">
            {/* Connection Status Icon */}
            <div className="flex items-center">
              {isOnline ? (
                <Wifi className={`h-4 w-4 ${quality.textColor}`} />
              ) : (
                <WifiOff className="h-4 w-4 text-red-500" />
              )}
            </div>
            
            {/* Status Indicator Dot */}
            <div className={`w-2 h-2 rounded-full ${quality.color} ${isOnline ? 'animate-pulse' : ''}`} />
            
            {/* Compact Status Badge */}
            <Badge 
              variant="outline" 
              className={`text-xs ${quality.textColor} border-current`}
            >
              {isOnline ? `${latency}ms` : 'Offline'}
            </Badge>
          </div>
        </TooltipTrigger>
        <TooltipContent side="bottom" className="p-0">
          <Card>
            <CardContent className="p-4 min-w-[200px]">
              <div className="space-y-3">
                <div className="flex items-center justify-between">
                  <span className="font-medium">Network Status</span>
                  <Badge className={quality.color}>
                    {quality.label}
                  </Badge>
                </div>
                
                <div className="space-y-2 text-sm">
                  <div className="flex items-center justify-between">
                    <div className="flex items-center gap-2">
                      <Globe className="h-3 w-3" />
                      <span>Connection</span>
                    </div>
                    <span className={isOnline ? 'text-green-600' : 'text-red-600'}>
                      {isOnline ? 'Online' : 'Offline'}
                    </span>
                  </div>
                  
                  {isOnline && (
                    <>
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2">
                          <Server className="h-3 w-3" />
                          <span>Latency</span>
                        </div>
                        <span className="font-mono">{latency}ms</span>
                      </div>
                      
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2">
                          <Wifi className="h-3 w-3" />
                          <span>Type</span>
                        </div>
                        <span className="capitalize">{connectionType}</span>
                      </div>
                      
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2">
                          <Clock className="h-3 w-3" />
                          <span>Updated</span>
                        </div>
                        <span className="text-xs">
                          {lastUpdate.toLocaleTimeString()}
                        </span>
                      </div>
                    </>
                  )}
                </div>
                
                <div className="pt-2 border-t text-xs text-muted-foreground">
                  Market data sync: {isOnline ? 'Active' : 'Paused'}
                </div>
              </div>
            </CardContent>
          </Card>
        </TooltipContent>
      </Tooltip>
    </TooltipProvider>
  );
}