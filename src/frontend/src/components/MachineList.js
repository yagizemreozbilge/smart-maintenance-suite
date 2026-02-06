import React from 'react';

// This component shows how to turn a Data Array into a Visual List
export function MachineList({ machines, filterStatus }) {

    // Point 2: Filter (Ayıklama)
    // We only show machines that match the filter, or all if filter is 'All'
    const filteredMachines = filterStatus === 'All'
        ? machines
        : machines.filter(m => m.status === filterStatus);

    return (
        <div className="machine-list">
            <h3>Active Machines ({filteredMachines.length})</h3>

            {/* Point 2: Map (Dönüştürme) */}
            <ul style={{ listStyle: 'none', padding: 0 }}>
                {filteredMachines.map((machine) => (
                    // Point 2: Key (TC Kimlik No) - CRITICAL for React
                    <li key={machine.id} style={{
                        padding: '10px',
                        borderBottom: '1px solid #ddd',
                        display: 'flex',
                        justifyContent: 'space-between'
                    }}>
                        <span><strong>{machine.name}</strong></span>
                        <span style={{
                            color: machine.status === 'Operational' ? 'green' : 'orange',
                            fontSize: '0.8em',
                            fontWeight: 'bold'
                        }}>
                            {machine.status.toUpperCase()}
                        </span>
                    </li>
                ))}
            </ul>

            {/* Point 6: Conditional Rendering for empty lists */}
            {filteredMachines.length === 0 && <p>No machines found for this category.</p>}
        </div>
    );
}
