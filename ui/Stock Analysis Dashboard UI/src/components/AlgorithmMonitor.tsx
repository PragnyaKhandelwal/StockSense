import { useState, useEffect } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from './ui/card';
import { Progress } from './ui/progress';
import { Badge } from './ui/badge';
import { Button } from './ui/button';
import { 
  Cpu, 
  HardDrive, 
  Activity,
  Clock,
  Zap,
  Database,
  BarChart3,
  GitBranch
} from 'lucide-react';

interface AlgorithmPerformance {
  name: string;
  complexity: string;
  memoryUsage: number;
  cpuUsage: number;
  executionTime: number;
  throughput: number;
  cacheHitRate: number;
  status: 'running' | 'idle' | 'optimizing';
}

export function AlgorithmMonitor() {
  const [algorithms, setAlgorithms] = useState<AlgorithmPerformance[]>([
    {
      name: 'Binary Search Tree',
      complexity: 'O(log n)',
      memoryUsage: 15,
      cpuUsage: 8,
      executionTime: 2.3,
      throughput: 450,
      cacheHitRate: 94,
      status: 'running'
    },
    {
      name: 'Hash Table Lookup',
      complexity: 'O(1)',
      memoryUsage: 22,
      cpuUsage: 5,
      executionTime: 0.8,
      throughput: 1200,
      cacheHitRate: 98,
      status: 'running'
    },
    {
      name: 'Graph Traversal',
      complexity: 'O(V + E)',
      memoryUsage: 35,
      cpuUsage: 18,
      executionTime: 8.7,
      throughput: 180,
      cacheHitRate: 87,
      status: 'optimizing'
    },
    {
      name: 'Dynamic Programming',
      complexity: 'O(n²)',
      memoryUsage: 28,
      cpuUsage: 12,
      executionTime: 15.2,
      throughput: 85,
      cacheHitRate: 91,
      status: 'running'
    }
  ]);

  const [totalCpuUsage, setTotalCpuUsage] = useState(0);
  const [totalMemoryUsage, setTotalMemoryUsage] = useState(0);

  useEffect(() => {
    const cpu = algorithms.reduce((acc, alg) => acc + alg.cpuUsage, 0);
    const memory = algorithms.reduce((acc, alg) => acc + alg.memoryUsage, 0);
    setTotalCpuUsage(Math.min(cpu, 100));
    setTotalMemoryUsage(Math.min(memory, 100));

    // Simulate real-time updates
    const interval = setInterval(() => {
      setAlgorithms(prev => prev.map(alg => ({
        ...alg,
        cpuUsage: Math.max(1, alg.cpuUsage + (Math.random() - 0.5) * 3),
        memoryUsage: Math.max(5, alg.memoryUsage + (Math.random() - 0.5) * 2),
        executionTime: Math.max(0.1, alg.executionTime + (Math.random() - 0.5) * 0.5)
      })));
    }, 2000);

    return () => clearInterval(interval);
  }, [algorithms]);

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'running': return 'bg-green-500';
      case 'optimizing': return 'bg-yellow-500';
      case 'idle': return 'bg-gray-500';
      default: return 'bg-gray-500';
    }
  };

  const getComplexityColor = (complexity: string) => {
    if (complexity.includes('O(1)') || complexity.includes('O(log n)')) return 'text-green-600';
    if (complexity.includes('O(n)') && !complexity.includes('²')) return 'text-yellow-600';
    return 'text-orange-600';
  };

  const getPerformanceColor = (value: number, threshold: { good: number; fair: number }) => {
    if (value >= threshold.good) return 'text-green-600';
    if (value >= threshold.fair) return 'text-yellow-600';
    return 'text-red-600';
  };

  return (
    <Card>
      <CardHeader>
        <div className="flex items-center justify-between">
          <CardTitle className="flex items-center gap-2">
            <Activity className="h-5 w-5" />
            Algorithm Performance Monitor
          </CardTitle>
          <div className="flex gap-2">
            <Badge variant="outline" className="flex items-center gap-1">
              <Cpu className="h-3 w-3" />
              CPU: {totalCpuUsage.toFixed(1)}%
            </Badge>
            <Badge variant="outline" className="flex items-center gap-1">
              <HardDrive className="h-3 w-3" />
              RAM: {totalMemoryUsage.toFixed(1)}%
            </Badge>
          </div>
        </div>
      </CardHeader>
      <CardContent className="space-y-6">
        {/* System Overview */}
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          <div className="text-center">
            <div className="text-2xl font-bold text-green-600">{algorithms.filter(a => a.status === 'running').length}</div>
            <div className="text-xs text-muted-foreground">Active</div>
          </div>
          <div className="text-center">
            <div className="text-2xl font-bold text-yellow-600">{algorithms.filter(a => a.status === 'optimizing').length}</div>
            <div className="text-xs text-muted-foreground">Optimizing</div>
          </div>
          <div className="text-center">
            <div className="text-2xl font-bold text-blue-600">{algorithms.reduce((acc, alg) => acc + alg.throughput, 0)}</div>
            <div className="text-xs text-muted-foreground">Ops/sec</div>
          </div>
          <div className="text-center">
            <div className="text-2xl font-bold text-purple-600">
              {(algorithms.reduce((acc, alg) => acc + alg.cacheHitRate, 0) / algorithms.length).toFixed(1)}%
            </div>
            <div className="text-xs text-muted-foreground">Cache Hit</div>
          </div>
        </div>

        {/* Algorithm Details */}
        <div className="space-y-3">
          {algorithms.map((algorithm, index) => (
            <div key={index} className="p-4 border border-border rounded-lg hover:bg-accent/50 transition-colors">
              <div className="flex items-center justify-between mb-3">
                <div className="flex items-center gap-3">
                  <div className="flex items-center gap-2">
                    {algorithm.name.includes('Binary') && <GitBranch className="h-4 w-4" />}
                    {algorithm.name.includes('Hash') && <Database className="h-4 w-4" />}
                    {algorithm.name.includes('Graph') && <Activity className="h-4 w-4" />}
                    {algorithm.name.includes('Dynamic') && <BarChart3 className="h-4 w-4" />}
                    <span className="font-medium">{algorithm.name}</span>
                  </div>
                  <Badge className={getStatusColor(algorithm.status)} variant="secondary">
                    {algorithm.status}
                  </Badge>
                </div>
                <div className="flex items-center gap-3">
                  <span className={`text-sm font-mono ${getComplexityColor(algorithm.complexity)}`}>
                    {algorithm.complexity}
                  </span>
                  <span className="text-sm text-muted-foreground">
                    {algorithm.executionTime.toFixed(1)}ms
                  </span>
                </div>
              </div>

              <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
                <div>
                  <div className="flex justify-between text-xs mb-1">
                    <span>CPU</span>
                    <span className={getPerformanceColor(algorithm.cpuUsage, { good: 80, fair: 60 })}>
                      {algorithm.cpuUsage.toFixed(1)}%
                    </span>
                  </div>
                  <Progress value={algorithm.cpuUsage} className="h-1" />
                </div>
                <div>
                  <div className="flex justify-between text-xs mb-1">
                    <span>Memory</span>
                    <span className={getPerformanceColor(algorithm.memoryUsage, { good: 80, fair: 60 })}>
                      {algorithm.memoryUsage.toFixed(1)}%
                    </span>
                  </div>
                  <Progress value={algorithm.memoryUsage} className="h-1" />
                </div>
                <div>
                  <div className="flex justify-between text-xs mb-1">
                    <span>Throughput</span>
                    <span className="text-blue-600">{algorithm.throughput} ops/s</span>
                  </div>
                  <Progress value={(algorithm.throughput / 1200) * 100} className="h-1" />
                </div>
                <div>
                  <div className="flex justify-between text-xs mb-1">
                    <span>Cache Hit</span>
                    <span className="text-green-600">{algorithm.cacheHitRate}%</span>
                  </div>
                  <Progress value={algorithm.cacheHitRate} className="h-1" />
                </div>
              </div>
            </div>
          ))}
        </div>

        {/* Performance Summary */}
        <div className="pt-4 border-t">
          <h4 className="font-medium mb-3 flex items-center gap-2">
            <Zap className="h-4 w-4" />
            System Performance Summary
          </h4>
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4 text-sm">
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">Total CPU:</span>
              <span className={getPerformanceColor(totalCpuUsage, { good: 80, fair: 60 })}>
                {totalCpuUsage.toFixed(1)}%
              </span>
            </div>
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">Total Memory:</span>
              <span className={getPerformanceColor(totalMemoryUsage, { good: 80, fair: 60 })}>
                {totalMemoryUsage.toFixed(1)}%
              </span>
            </div>
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">Avg Latency:</span>
              <span className="text-green-600">
                {(algorithms.reduce((acc, alg) => acc + alg.executionTime, 0) / algorithms.length).toFixed(1)}ms
              </span>
            </div>
            <div className="flex items-center justify-between">
              <span className="text-muted-foreground">System Status:</span>
              <Badge className="bg-green-500">Optimal</Badge>
            </div>
          </div>
        </div>
      </CardContent>
    </Card>
  );
}