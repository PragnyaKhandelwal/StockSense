import { useState, useEffect } from 'react';
import { Header } from './components/Header';
import { StockSidebar } from './components/StockSidebar';
import { Dashboard } from './components/Dashboard';
import { Predictions } from './components/Predictions';
import { Watchlist } from './components/Watchlist';
import { Settings } from './components/Settings';

export default function App() {
  const [darkMode, setDarkMode] = useState(false);
  const [currentView, setCurrentView] = useState('dashboard');
  const [selectedStock, setSelectedStock] = useState('RELIANCE');

  useEffect(() => {
    if (darkMode) {
      document.documentElement.classList.add('dark');
    } else {
      document.documentElement.classList.remove('dark');
    }
    
    // Update page title and favicon
    document.title = 'StockSense - Professional Stock Analysis';
  }, [darkMode]);

  const renderContent = () => {
    switch (currentView) {
      case 'predictions':
        return <Predictions selectedStock={selectedStock} />;
      case 'watchlist':
        return <Watchlist onSelectStock={setSelectedStock} />;
      case 'settings':
        return <Settings />;
      default:
        return <Dashboard selectedStock={selectedStock} onSelectStock={setSelectedStock} />;
    }
  };

  return (
    <div className="h-screen bg-background text-foreground flex flex-col">
      <Header 
        darkMode={darkMode} 
        setDarkMode={setDarkMode}
        onSelectStock={setSelectedStock}
      />
      <div className="flex flex-1 overflow-hidden">
        <StockSidebar 
          currentView={currentView} 
          setCurrentView={setCurrentView}
        />
        <main className="flex-1 overflow-auto">
          {renderContent()}
        </main>
      </div>
    </div>
  );
}