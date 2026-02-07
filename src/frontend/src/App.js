import React, { useState, useEffect } from 'react';
import { MaintenanceProvider, useMaintenance, useMaintenanceDispatch } from './context/MaintenanceContext.js';
import { useMachineStats } from './context/useHooks.js';
import WelcomeMessage, { Header } from './components/Header.js';
import { StatusCard } from './components/StatusCard.js';
import { MachineList } from './components/MachineList.js';
import { MaintenanceForm } from './components/MaintenanceForm.js';

function Dashboard() {
    const { stock, machines } = useMaintenance();
    const dispatch = useMaintenanceDispatch();

    // Point 5: Using Custom Hook for calculations
    const stats = useMachineStats();

    // Point 2: useEffect for Simulation / Real Fetch
    const [isSyncing, setIsSyncing] = useState(false);

    useEffect(() => {
        // Simulated fetching from C Backend
        setIsSyncing(true);
        const timeout = setTimeout(() => {
            setIsSyncing(false);
            console.log("System data synchronized with C backend.");
        }, 1500);

        return () => clearTimeout(timeout);
    }, [machines]); // Re-sync when machines list changes

    return (
        <div style={{ padding: '20px', maxWidth: '1200px', margin: '0 auto', backgroundColor: '#f4f7f6', minHeight: '100vh', fontFamily: 'Inter, sans-serif' }}>
            <Header title="Smart Maintenance Control" />
            <WelcomeMessage />

            {/* Stats Bar - Demonstrating useMemo result */}
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: '15px', margin: '20px 0' }}>
                <StatTile label="Total Assets" value={stats.total} color="#3498db" />
                <StatTile label="Healthy" value={stats.operational} color="#2ecc71" />
                <StatTile label="Warnings" value={stats.warning} color="#f1c40f" />
                <StatTile label="Health Score" value={`${stats.healthScore}%`} color="#9b59b6" />
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: '350px 1fr', gap: '30px' }}>

                <aside>
                    <StatusCard
                        label="Oil Inventory"
                        value={stock}
                        isLowStock={stock < 5}
                        onRestock={() => dispatch({ type: 'UPDATE_STOCK', payload: 15 })}
                    >
                        <p>Threshold monitoring active.</p>
                    </StatusCard>

                    <div style={{ marginTop: '20px' }}>
                        <MaintenanceForm />
                    </div>
                </aside>

                <main style={{ backgroundColor: 'white', padding: '25px', borderRadius: '12px', boxShadow: '0 4px 20px rgba(0,0,0,0.08)' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '10px' }}>
                        <h2 style={{ margin: 0 }}>Asset Telemetry</h2>
                        {isSyncing && <span style={{ fontSize: '0.8em', color: '#3498db', fontWeight: 'bold' }}>📡 SYNCING WITH BACKEND...</span>}
                    </div>
                    <MachineList />
                </main>

            </div>
        </div>
    );
}

// Simple internal component for layout
function StatTile({ label, value, color }) {
    return (
        <div style={{ backgroundColor: 'white', padding: '15px', borderRadius: '8px', borderTop: `4px solid ${color}`, boxShadow: '0 2px 4px rgba(0,0,0,0.05)' }}>
            <small style={{ color: '#7f8c8d', textTransform: 'uppercase', letterSpacing: '1px' }}>{label}</small>
            <div style={{ fontSize: '1.5em', fontWeight: 'bold', color: '#2c3e50' }}>{value}</div>
        </div>
    );
}

function App() {
    return (
        <MaintenanceProvider>
            <Dashboard />
        </MaintenanceProvider>
    );
}

export default App;
