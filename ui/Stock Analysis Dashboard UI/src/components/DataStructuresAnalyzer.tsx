import { useState } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Progress } from './ui/progress';
import { Badge } from './ui/badge';
import { Button } from './ui/button';
import { 
  TreePine, 
  Hash, 
  BarChart3, 
  GitBranch, 
  Clock,
  Zap,
  Target
} from 'lucide-react';

interface DataStructuresAnalyzerProps {
  selectedStock: string;
}

// Mock data structures implementations for analysis
const analysisData = {
  binarySearchTree: {
    name: 'BST Price Analysis',
    icon: TreePine,
    description: 'Binary search for optimal buy/sell points',
    efficiency: 'O(log n)',
    accuracy: 87,
    results: {
      buySignals: 3,
      sellSignals: 2,
      neutralZones: 1
    }
  },
  hashTable: {
    name: 'Hash-based Correlation',
    icon: Hash,
    description: 'Fast lookup of similar patterns',
    efficiency: 'O(1)',
    accuracy: 92,
    results: {
      similarPatterns: 15,
      strongCorrelations: 8,
      weakCorrelations: 7
    }
  },
  priorityQueue: {
    name: 'Priority Alert System',
    icon: Zap,
    description: 'Heap-based alert prioritization',
    efficiency: 'O(log n)',
    accuracy: 95,
    results: {
      highPriority: 2,
      mediumPriority: 5,
      lowPriority: 8
    }
  },
  graph: {
    name: 'Graph Network Analysis',
    icon: GitBranch,
    description: 'Sector correlation mapping',
    efficiency: 'O(V + E)',
    accuracy: 83,
    results: {
      connectedStocks: 12,
      strongEdges: 6,
      weakEdges: 18
    }
  },
  dynamicProgramming: {
    name: 'DP Optimization',
    icon: BarChart3,
    description: 'Maximum profit calculation',
    efficiency: 'O(n)',
    accuracy: 89,
    results: {
      maxProfit: '₹245.50',
      optimalDays: 7,
      transactions: 3
    }
  },
  timeComplexity: {
    name: 'Algorithm Performance',
    icon: Clock,
    description: 'Real-time complexity analysis',
    efficiency: 'O(n log n)',
    accuracy: 91,
    results: {
      avgProcessingTime: '12ms',
      memoryUsage: '2.3MB',
      throughput: '850 ops/sec'
    }
  }
};

export function DataStructuresAnalyzer({ selectedStock }: DataStructuresAnalyzerProps) {
  const [selectedAlgorithm, setSelectedAlgorithm] = useState('binarySearchTree');
  const [showPerformance, setShowPerformance] = useState(false);

  const currentAnalysis = analysisData[selectedAlgorithm as keyof typeof analysisData];
  const Icon = currentAnalysis.icon;

  const getEfficiencyColor = (efficiency: string) => {
    if (efficiency.includes('O(1)') || efficiency.includes('O(log n)')) return 'text-green-500';
    if (efficiency.includes('O(n)')) return 'text-yellow-500';
    return 'text-orange-500';
  };

  const getAccuracyColor = (accuracy: number) => {
    if (accuracy >= 90) return 'bg-green-500';
    if (accuracy >= 80) return 'bg-yellow-500';
    return 'bg-red-500';
  };

  return (
    <Card>
      <CardHeader>
        <div className="flex items-center justify-between">
          <CardTitle className="flex items-center gap-2">
            <Target className="h-5 w-5" />
            Data Structures & Algorithms Analysis
          </CardTitle>
          <Button
            variant="outline"
            size="sm"
            onClick={() => setShowPerformance(!showPerformance)}
          >
            {showPerformance ? 'Hide' : 'Show'} Performance
          </Button>
        </div>
      </CardHeader>
      <CardContent className="space-y-6">
        {/* Algorithm Selection Grid */}
        <div className="grid grid-cols-2 md:grid-cols-3 gap-3">
          {Object.entries(analysisData).map(([key, data]) => {
            const AlgIcon = data.icon;
            return (
              <div
                key={key}
                onClick={() => setSelectedAlgorithm(key)}
                className={`p-3 rounded-lg border cursor-pointer transition-all duration-200 hover:shadow-md ${
                  selectedAlgorithm === key 
                    ? 'border-primary bg-primary/10 shadow-md' 
                    : 'border-border hover:bg-accent'
                }`}
              >
                <div className="flex items-center gap-2 mb-2">
                  <AlgIcon className="h-4 w-4" />
                  <span className="font-medium text-sm">{data.name}</span>
                </div>
                <div className="text-xs text-muted-foreground">{data.description}</div>
                <div className={`text-xs font-mono mt-1 ${getEfficiencyColor(data.efficiency)}`}>
                  {data.efficiency}
                </div>
              </div>
            );
          })}
        </div>

        {/* Selected Algorithm Analysis */}
        <div className="border-t pt-6">
          <div className="flex items-center gap-3 mb-4">
            <div className="p-2 rounded-lg bg-primary/10">
              <Icon className="h-6 w-6 text-primary" />
            </div>
            <div>
              <h3 className="font-semibold">{currentAnalysis.name}</h3>
              <p className="text-sm text-muted-foreground">{currentAnalysis.description}</p>
            </div>
          </div>

          {/* Accuracy Meter */}
          <div className="mb-4">
            <div className="flex justify-between items-center mb-2">
              <span className="text-sm font-medium">Algorithm Accuracy</span>
              <Badge className={getAccuracyColor(currentAnalysis.accuracy)}>
                {currentAnalysis.accuracy}%
              </Badge>
            </div>
            <Progress value={currentAnalysis.accuracy} className="h-3" />
          </div>

          {/* Time Complexity Badge */}
          <div className="mb-4">
            <div className="flex items-center gap-2">
              <span className="text-sm text-muted-foreground">Time Complexity:</span>
              <Badge 
                variant="outline" 
                className={`font-mono ${getEfficiencyColor(currentAnalysis.efficiency)}`}
              >
                {currentAnalysis.efficiency}
              </Badge>
            </div>
          </div>

          {/* Analysis Results */}
          <div className="space-y-2">
            <h4 className="font-medium text-sm">Analysis Results:</h4>
            <div className="grid grid-cols-1 md:grid-cols-3 gap-3">
              {Object.entries(currentAnalysis.results).map(([key, value]) => (
                <div key={key} className="p-3 bg-muted/50 rounded-lg">
                  <div className="text-xs text-muted-foreground capitalize mb-1">
                    {key.replace(/([A-Z])/g, ' $1').trim()}
                  </div>
                  <div className="font-semibold">{value}</div>
                </div>
              ))}
            </div>
          </div>

          {/* Performance Metrics */}
          {showPerformance && (
            <div className="mt-4 p-4 bg-muted/30 rounded-lg">
              <h4 className="font-medium text-sm mb-3">Performance Metrics</h4>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4 text-sm">
                <div>
                  <span className="text-muted-foreground">Memory Usage:</span>
                  <div className="font-mono">2.3MB / 8MB allocated</div>
                  <Progress value={28} className="h-1 mt-1" />
                </div>
                <div>
                  <span className="text-muted-foreground">CPU Usage:</span>
                  <div className="font-mono">15% / 100% available</div>
                  <Progress value={15} className="h-1 mt-1" />
                </div>
                <div>
                  <span className="text-muted-foreground">Cache Hit Rate:</span>
                  <div className="font-mono text-green-600">94.2%</div>
                </div>
                <div>
                  <span className="text-muted-foreground">Avg Response:</span>
                  <div className="font-mono text-green-600">12ms</div>
                </div>
              </div>
            </div>
          )}
        </div>
      </CardContent>
    </Card>
  );
}