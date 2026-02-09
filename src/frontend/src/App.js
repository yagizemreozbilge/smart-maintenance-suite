import React, { useState, useEffect } from 'react';
import { MaintenanceProvider, useMaintenance, useMaintenanceDispatch } from './context/MaintenanceContext.js';
import { useMachineStats } from './context/useHooks.js';
import WelcomeMessage, { Header } from './components/Header.js';
import { MachineList } from './components/MachineList.js';
import { MaintenanceForm } from './components/MaintenanceForm.js';
import { AlertsPanel } from './components/AlertsPanel.js';
import LoginPage from './components/LoginPage.js';
import { InventoryPanel } from './components/InventoryPanel.js';
import { MaintenanceHistory } from './components/MaintenanceHistory.js';

function Dashboard() {
    const { machines = [], alerts = [], maintenanceLogs = [] } = useMaintenance();
    const dispatch = useMaintenanceDispatch();
    const stats = useMachineStats();
    const [isSyncing, setIsSyncing] = useState(false);

    const syncWithBackend = async () => {
        setIsSyncing(true);
        try {
            const [mRes, iRes, aRes, maintRes] = await Promise.all([
                fetch('http://127.0.0.1:8080/api/machines'),
                fetch('http://127.0.0.1:8080/api/inventory'),
                fetch('http://127.0.0.1:8080/api/alerts'),
                fetch('http://127.0.0.1:8080/api/maintenance')
            ]);

            const machinesData = await mRes.json();
            const inventoryData = await iRes.json();
            const alertsData = await aRes.json();
            const maintData = await maintRes.json();

            dispatch({ type: 'SET_MACHINES', payload: machinesData || [] });
            dispatch({ type: 'SET_INVENTORY', payload: inventoryData || [] });
            dispatch({ type: 'SET_ALERTS', payload: alertsData || [] });
            dispatch({ type: 'SET_MAINTENANCE_LOGS', payload: maintData || [] });

            console.log("System data synchronized with C backend.");
        } catch (error) {
            console.error("Backend connection failed:", error);
        } finally {
            setIsSyncing(false);
        }
    };

    useEffect(() => {
        syncWithBackend();
        const interval = setInterval(syncWithBackend, 30000);
        return () => clearInterval(interval);
    }, [dispatch]);

    return (
        <div style={{ padding: '20px', maxWidth: '1440px', margin: '0 auto', backgroundColor: '#f4f7f6', minHeight: '100vh', fontFamily: 'Inter, sans-serif' }}>
            <Header title="Smart Maintenance Control" />
            <WelcomeMessage />

            {localStorage.getItem('user_role') === 'admin' && (
                <div style={{ display: 'flex', gap: '15px', marginBottom: '15px' }}>
                    <button
                        onClick={() => window.open('http://127.0.0.1:8080/api/reports/maintenance')}
                        style={{ backgroundColor: '#2c3e50', color: 'white', border: 'none', padding: '10px 15px', borderRadius: '6px', cursor: 'pointer', fontWeight: 'bold', fontSize: '0.8rem' }}
                    >
                        📊 DOWNLOAD MAINTENANCE LOGS (.CSV)
                    </button>
                    <button
                        onClick={() => window.open('http://127.0.0.1:8080/api/reports/inventory')}
                        style={{ backgroundColor: '#2c3e50', color: 'white', border: 'none', padding: '10px 15px', borderRadius: '6px', cursor: 'pointer', fontWeight: 'bold', fontSize: '0.8rem' }}
                    >
                        📦 DOWNLOAD INVENTORY REPORT (.CSV)
                    </button>
                    <div style={{ display: 'flex', alignItems: 'center', marginLeft: 'auto', color: '#7f8c8d', fontSize: '0.8rem' }}>
                        * Exporting directly from PostgreSQL backend
                    </div>
                </div>
            )}

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(4, 1fr)', gap: '15px', margin: '20px 0' }}>
                <StatTile label="Total Assets" value={stats?.total || 0} color="#3498db" />
                <StatTile label="Healthy" value={stats?.operational || 0} color="#2ecc71" />
                <StatTile label="Critical Issues" value={alerts.filter(a => a.severity === 'CRITICAL').length} color="#e74c3c" />
                <StatTile label="Recent Logs" value={maintenanceLogs.length} color="#9b59b6" />
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: '320px 1fr 340px', gap: '20px' }}>
                <aside style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
                    <div style={{ backgroundColor: 'white', padding: '15px', borderRadius: '12px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)' }}>
                        <InventoryPanel />
                    </div>
                </aside>

                <main style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
                    <div style={{ backgroundColor: 'white', padding: '25px', borderRadius: '12px', boxShadow: '0 4px 20px rgba(0,0,0,0.08)' }}>
                        <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '20px' }}>
                            <h2 style={{ margin: 0 }}>Asset Telemetry</h2>
                            {isSyncing && <span style={{ fontSize: '0.8em', color: '#3498db', fontWeight: 'bold' }}>📡 SYNCING...</span>}
                        </div>
                        <MachineList />
                    </div>
                    <MaintenanceHistory />
                </main>

                <aside style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
                    {localStorage.getItem('user_role') !== 'operator' && <MaintenanceForm />}
                    <div style={{ backgroundColor: 'white', padding: '15px', borderRadius: '12px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)' }}>
                        <AlertsPanel />
                    </div>
                </aside>
            </div>
        </div>
    );
}

function StatTile({ label, value, color }) {
    return (
        <div style={{ backgroundColor: 'white', padding: '15px', borderRadius: '8px', borderTop: `4px solid ${color}`, boxShadow: '0 2px 4px rgba(0,0,0,0.05)' }}>
            <small style={{ color: '#7f8c8d', textTransform: 'uppercase', letterSpacing: '1px' }}>{label}</small>
            <div style={{ fontSize: '1.5em', fontWeight: 'bold', color: '#2c3e50' }}>{value}</div>
        </div>
    );
}

function App() {
    const [isAuthenticated, setIsAuthenticated] = useState(false);

    useEffect(() => {
        const token = localStorage.getItem('auth_token');
        if (token) {
            setIsAuthenticated(true);
        }
    }, []);

    const handleLogin = () => {
        setIsAuthenticated(true);
    };

    const handleLogout = () => {
        localStorage.clear();
        setIsAuthenticated(false);
    };

    return (
        <MaintenanceProvider>
            {!isAuthenticated ? (
                <LoginPage onLogin={handleLogin} />
            ) : (
                <div className="App">
                    <div style={{ textAlign: 'right', padding: '10px', backgroundColor: '#2c3e50' }}>
                        <button
                            onClick={handleLogout}
                            style={{ backgroundColor: '#e74c3c', color: 'white', border: 'none', padding: '8px 15px', borderRadius: '5px', cursor: 'pointer', fontWeight: 'bold' }}
                        >
                            SİSTEMDEN ÇIKIŞ YAP 🔓
                        </button>
                    </div>
                    <Dashboard />
                </div>
            )}
        </MaintenanceProvider>
    );
}

export default App;
