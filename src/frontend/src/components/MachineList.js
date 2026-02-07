import React, { useState } from 'react';
import { useMaintenance, useMaintenanceDispatch } from '../context/MaintenanceContext.js';

export function MachineList() {
    // Point 3: Sharing State - Accessing central data
    const { machines } = useMaintenance();
    const dispatch = useMaintenanceDispatch();

    const [filterStatus, setFilterStatus] = useState('All');

    const filteredMachines = filterStatus === 'All'
        ? machines
        : machines.filter(m => m.status === filterStatus);

    return (
        <div className="machine-list" style={{ marginTop: '20px' }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <h3>Monitoring Unit ({filteredMachines.length})</h3>
                <div>
                    <select onChange={(e) => setFilterStatus(e.target.value)} style={{ padding: '5px' }}>
                        <option value="All">Show All</option>
                        <option value="Operational">Operational</option>
                        <option value="Warning">Warning</option>
                        <option value="Maintenance">Maintenance</option>
                    </select>
                </div>
            </div>

            <ul style={{ listStyle: 'none', padding: 0 }}>
                {filteredMachines.map((machine) => (
                    <li key={machine.id} style={{
                        padding: '12px',
                        borderBottom: '1px solid #eee',
                        display: 'flex',
                        justifyContent: 'space-between',
                        alignItems: 'center'
                    }}>
                        <span><strong>{machine.name}</strong></span>
                        <div>
                            <span style={{
                                color: machine.status === 'Operational' ? '#2ecc71' : '#f39c12',
                                marginRight: '15px',
                                fontWeight: 'bold'
                            }}>
                                {machine.status.toUpperCase()}
                            </span>
                            {/* Point 5: Triggering logic change via reducer */}
                            <button
                                onClick={() => dispatch({ type: 'REMOVE_MACHINE', payload: machine.id })}
                                style={{ color: 'red', border: 'none', background: 'none', cursor: 'pointer' }}
                            >
                                Delete
                            </button>
                        </div>
                    </li>
                ))}
            </ul>
        </div>
    );
}
