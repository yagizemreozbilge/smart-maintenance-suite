import React, { useState } from 'react';
import { useMaintenance, useMaintenanceDispatch } from '../context/MaintenanceContext.js';
import { useLiveSearch } from '../context/useHooks.js';

export function MachineList() {
    const { machines } = useMaintenance();
    const dispatch = useMaintenanceDispatch();
    const [filterStatus, setFilterStatus] = useState('All');

    // Point 5: Using Custom Hook for search logic
    // Point 4: Inside useLiveSearch, useMemo is handling expensive filtering
    const { query, setQuery, filteredItems } = useLiveSearch(machines, 'name');

    const finalItems = filterStatus === 'All'
        ? filteredItems
        : filteredItems.filter(m => m.status === filterStatus);

    return (
        <div className="machine-list">
            <h2 style={{ marginBottom: '15px' }}>Assets Inventory</h2>
            <div style={{ marginBottom: '20px' }}>
                <input
                    type="text"
                    placeholder="Search assets..."
                    value={query}
                    onChange={(e) => setQuery(e.target.value)}
                    style={{ width: '100%', padding: '10px', borderRadius: '4px', border: '1px solid #ddd', marginBottom: '10px' }}
                />
                <div style={{ display: 'flex', gap: '5px' }}>
                    {['All', 'Operational', 'Warning', 'Maintenance'].map(s => (
                        <button
                            key={s}
                            onClick={() => setFilterStatus(s)}
                            style={{
                                padding: '5px 10px',
                                backgroundColor: filterStatus === s ? '#3498db' : '#eee',
                                color: filterStatus === s ? 'white' : 'black',
                                border: 'none',
                                borderRadius: '4px',
                                cursor: 'pointer'
                            }}
                        >
                            {s}
                        </button>
                    ))}
                </div>
            </div>

            <div style={{ maxHeight: '600px', overflowY: 'auto' }}>
                {finalItems.map((machine) => (
                    <MachineCard key={machine.id} machine={machine} />
                ))}
            </div>
        </div>
    );
}

function MachineCard({ machine }) {
    const dispatch = useMaintenanceDispatch();
    const [showSensors, setShowSensors] = useState(false);
    const [sensors, setSensors] = useState([]);
    const [loading, setLoading] = useState(false);

    const toggleSensors = async () => {
        if (!showSensors) {
            setLoading(true);
            try {
                const res = await fetch(`http://localhost:8080/api/sensors?id=${machine.id}`);
                const data = await res.json();
                setSensors(data);
            } catch (err) {
                console.error("Sensor fetch error:", err);
            } finally {
                setLoading(false);
            }
        }
        setShowSensors(!showSensors);
    };

    return (
        <div style={{
            padding: '15px',
            marginBottom: '15px',
            backgroundColor: '#f8f9fa',
            borderRadius: '10px',
            borderLeft: `6px solid ${machine.status === 'operational' ? '#2ecc71' : machine.status === 'warning' ? '#f1c40f' : '#e74c3c'}`,
            boxShadow: '0 2px 4px rgba(0,0,0,0.05)'
        }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <div>
                    <h4 style={{ margin: 0, fontSize: '1.1rem' }}>{machine.name}</h4>
                    <div style={{ fontSize: '0.8rem', color: '#7f8c8d' }}>📍 {machine.location} | ID: {machine.id}</div>
                </div>
                <div style={{ textAlign: 'right' }}>
                    <span style={{
                        padding: '4px 8px',
                        borderRadius: '4px',
                        backgroundColor: machine.status === 'operational' ? '#e1f9e1' : '#fff4e5',
                        color: machine.status === 'operational' ? '#1e7e34' : '#d39e00',
                        fontSize: '0.75rem',
                        fontWeight: 'bold',
                        textTransform: 'uppercase'
                    }}>
                        {machine.status}
                    </span>
                    <div style={{ marginTop: '10px' }}>
                        <button
                            onClick={toggleSensors}
                            style={{ fontSize: '0.75rem', marginRight: '10px', border: 'none', background: 'none', color: '#3498db', cursor: 'pointer', fontWeight: 'bold' }}
                        >
                            {showSensors ? 'CLOSE TELEMETRY' : 'VIEW SENSORS'}
                        </button>
                        {localStorage.getItem('user_role') === 'admin' && (
                            <button
                                onClick={() => dispatch({ type: 'REMOVE_MACHINE', payload: machine.id })}
                                style={{ color: '#e74c3c', border: 'none', background: 'none', cursor: 'pointer', fontSize: '0.75rem' }}
                            >
                                Deactivate
                            </button>
                        )}
                    </div>
                </div>
            </div>

            {showSensors && (
                <div style={{ marginTop: '15px', padding: '10px', backgroundColor: '#fff', borderRadius: '6px', border: '1px solid #eee' }}>
                    <h5 style={{ margin: '0 0 10px 0', fontSize: '0.8rem', color: '#666' }}>LIVE TELEMETRY</h5>
                    {loading ? (
                        <p style={{ fontSize: '0.8rem' }}>Loading sensors...</p>
                    ) : sensors.length > 0 ? (
                        <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '10px' }}>
                            {sensors.map(s => (
                                <div key={s.sensor_id} style={{ padding: '8px', border: '1px solid #f0f0f0', borderRadius: '4px' }}>
                                    <div style={{ fontSize: '0.7rem', color: '#95a5a6' }}>{s.type.toUpperCase()}</div>
                                    <div style={{ fontSize: '1rem', fontWeight: 'bold' }}>{s.value} <span style={{ fontSize: '0.7rem' }}>{s.unit}</span></div>
                                </div>
                            ))}
                        </div>
                    ) : (
                        <p style={{ fontSize: '0.8rem', color: '#999' }}>No active sensors for this unit.</p>
                    )}
                </div>
            )}
        </div>
    );
}
