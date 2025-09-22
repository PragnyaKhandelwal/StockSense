import { TrendingUp } from 'lucide-react';

interface AppIconProps {
  size?: number;
  className?: string;
}

export function AppIcon({ size = 32, className = '' }: AppIconProps) {
  return (
    <div className={`relative ${className}`} style={{ width: size, height: size }}>
      {/* Gradient Background */}
      <div className="absolute inset-0 bg-gradient-to-br from-blue-600 via-purple-600 to-blue-800 rounded-lg shadow-lg">
        {/* Inner glow */}
        <div className="absolute inset-0 bg-gradient-to-br from-blue-400/30 to-transparent rounded-lg"></div>
      </div>
      
      {/* Icon */}
      <div className="relative h-full w-full flex items-center justify-center">
        <TrendingUp 
          className="text-white drop-shadow-sm" 
          style={{ width: size * 0.6, height: size * 0.6 }}
        />
      </div>
      
      {/* Shine effect */}
      <div className="absolute top-1 left-1 w-1/2 h-1/2 bg-gradient-to-br from-white/40 to-transparent rounded-tl-lg"></div>
    </div>
  );
}