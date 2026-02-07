import React from 'react';
import { MaintenanceProvider, useMaintenance, useMaintenanceDispatch } from './context/MaintenanceContext.js';
import WelcomeMessage, { Header } from './components/Header.js';
import { StatusCard } from './components/StatusCard.js';
import { MachineList } from './components/MachineList.js';
import { MaintenanceForm } from './components/MaintenanceForm.js';

function Dashboard() {
    // Point 7: Accessing state and dispatch anywhere
    const { stock } = useMaintenance();
    const dispatch = useMaintenanceDispatch();

    return (
        <div style={{ padding: '20px', maxWidth: '1000px', margin: '0 auto', fontFamily: 'sans-serif' }}>
            <Header title="Professional Maintenance Suite" />
            <WelcomeMessage />

            <div style={{ display: 'grid', gridTemplateColumns: '1fr 1.5fr', gap: '30px', marginTop: '30px' }}>

                {/* Left Column: Alerts & Manual Actions */}
                <section>
                    {/* Point 3: Sending state updates to the shared store */}
                    <StatusCard
                        label="Oil Pressure System"
                        value={stock}
                        isLowStock={stock < 5}
                        onRestock={() => dispatch({ type: 'UPDATE_STOCK', payload: 15 })}
                    >
                        <p>Critical threshold: 5 units</p>
                    </StatusCard>

                    <div style={{ marginTop: '20px' }}>
                        {/* Point 1, 4: Form state is local, but affects global via dispatch */}
                        {/* Point 4: Key could be used here to reset the whole form if needed */}
                        <MaintenanceForm key="main-form" />
                    </div>
                </section>

                {/* Right Column: Global Monitoring */}
                <section style={{ backgroundColor: 'white', padding: '20px', borderRadius: '12px', boxShadow: '0 4px 6px rgba(0,0,0,0.1)' }}>
                    <MachineList />
                </section>

            </div>

            <footer style={{ marginTop: '50px', borderTop: '1px solid #eee', paddingTop: '20px', textAlign: 'center' }}>
                {/* Point 2: Computed information from state */}
                <p style={{ color: stock < 5 ? 'red' : 'green', fontWeight: 'bold' }}>
                    System Health Index: {stock < 5 ? 'ATTENTION REQUIRED' : 'OPTIMAL'}
                </p>
            </footer>
        </div>
    );
}

// Point 7: The Top Level Provider
function App() {
    return (
        <MaintenanceProvider>
            <Dashboard />
        </MaintenanceProvider>
    );
}

export default App;
