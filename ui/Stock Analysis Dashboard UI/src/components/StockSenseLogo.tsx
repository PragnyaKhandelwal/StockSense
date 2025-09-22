import { TrendingUp } from 'lucide-react';

interface StockSenseLogoProps {
  size?: 'sm' | 'md' | 'lg';
  variant?: 'full' | 'icon' | 'text';
  className?: string;
}

export function StockSenseLogo({ 
  size = 'md', 
  variant = 'full', 
  className = '' 
}: StockSenseLogoProps) {
  const sizeClasses = {
    sm: {
      icon: 'h-5 w-5',
      text: 'text-lg',
      container: 'gap-2'
    },
    md: {
      icon: 'h-8 w-8',
      text: 'text-xl',
      container: 'gap-2'
    },
    lg: {
      icon: 'h-12 w-12',
      text: 'text-3xl',
      container: 'gap-3'
    }
  };

  const currentSize = sizeClasses[size];

  const LogoIcon = () => (
    <div className="relative">
      <div className="absolute inset-0 bg-gradient-to-br from-blue-500 to-purple-600 rounded-lg opacity-20 blur-sm"></div>
      <div className="relative bg-gradient-to-br from-blue-600 to-purple-700 p-1.5 rounded-lg shadow-lg">
        <TrendingUp className={`${currentSize.icon} text-white`} />
      </div>
    </div>
  );

  const LogoText = () => (
    <div className="flex flex-col">
      <span className={`font-bold bg-gradient-to-r from-blue-600 to-purple-700 bg-clip-text text-transparent ${currentSize.text}`}>
        StockSense
      </span>
      {size === 'lg' && (
        <span className="text-xs text-muted-foreground font-medium tracking-wider uppercase">
          Professional Analysis
        </span>
      )}
    </div>
  );

  if (variant === 'icon') {
    return <LogoIcon />;
  }

  if (variant === 'text') {
    return <LogoText />;
  }

  return (
    <div className={`flex items-center ${currentSize.container} ${className}`}>
      <LogoIcon />
      <LogoText />
    </div>
  );
}