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

            <div style={{ maxHeight: '400px', overflowY: 'auto' }}>
                {finalItems.map((machine) => (
                    <div key={machine.id} style={{
                        padding: '15px',
                        marginBottom: '10px',
                        backgroundColor: '#f8f9fa',
                        borderRadius: '6px',
                        display: 'flex',
                        justifyContent: 'space-between',
                        alignItems: 'center',
                        borderLeft: `5px solid ${machine.status === 'Operational' ? '#2ecc71' : machine.status === 'Warning' ? '#f1c40f' : '#e74c3c'}`
                    }}>
                        <div>
                            <h4 style={{ margin: 0 }}>{machine.name}</h4>
                            <small style={{ color: '#666' }}>ID: {machine.id}</small>
                        </div>
                        <div style={{ textAlign: 'right' }}>
                            <span style={{ display: 'block', fontWeight: 'bold', fontSize: '0.8em', marginBottom: '5px' }}>
                                {machine.status.toUpperCase()}
                            </span>
                            <button
                                onClick={() => dispatch({ type: 'REMOVE_MACHINE', payload: machine.id })}
                                style={{ color: '#e74c3c', border: 'none', background: 'none', cursor: 'pointer', fontSize: '0.8em' }}
                            >
                                Deactivate
                            </button>
                        </div>
                    </div>
                ))}
                {finalItems.length === 0 && <p style={{ textAlign: 'center', color: '#999' }}>No assets match your criteria.</p>}
            </div>
        </div>
    );
}
