// Point 2: Importing (Named and Default)
import WelcomeMessage, { Header } from './components/Header';
import { StatusCard } from './components/StatusCard';

// Point 1: Root Component (TitleCase)
function App() {
    const machineName = "CNC Lathe-01";
    const stockLevel = 3;
    const minRequired = 5;

    return (
        // Point 3: Single Parent (empty fragment <> )
        <>
            <Header title="Smart Maintenance Dashboard" />
            <WelcomeMessage />

            <main>
                {/* Point 4 & 5: Passing dynamic values as props */}
                <StatusCard
                    label="Part: Cooling Pump"
                    value={stockLevel}
                    isLowStock={stockLevel < minRequired}
                >
                    {/* Point 5: This content goes into the "children" prop */}
                    <p>Location: Block A / Shelf 4</p>
                    <small>Managed by: {machineName}</small>
                </StatusCard>

                {/* Another example of Conditional Rendering at App level */}
                {stockLevel === 0 ? (
                    <p style={{ color: 'red', fontWeight: 'bold' }}>SYSTEM CRITICAL: NO STOCK!</p>
                ) : (
                    <p>System operational.</p>
                )}
            </main>
        </>
    );
}

// Point 2: Default Export
export default App;
