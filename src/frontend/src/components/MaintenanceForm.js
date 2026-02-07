import React, { useState } from 'react';
import { useMaintenanceDispatch } from '../context/MaintenanceContext.js';

export function MaintenanceForm() {
    // Point 1: Reacting to Input with state
    // Point 2: Simple structure
    const [name, setName] = useState('');
    const [status, setStatus] = useState('Operational');
    const dispatch = useMaintenanceDispatch();

    const handleSubmit = (e) => {
        e.preventDefault();
        if (!name) return;

        // Point 5: Dispatching an action
        dispatch({
            type: 'ADD_MACHINE',
            payload: { id: Date.now(), name, status }
        });

        setName(''); // Reset local state
    };

    return (
        <form onSubmit={handleSubmit} style={{ padding: '15px', border: '1px dashed #666', borderRadius: '8px' }}>
            <h4>Add New Equipment</h4>
            <input
                type="text"
                placeholder="Machine Name"
                value={name}
                onChange={(e) => setName(e.target.value)}
                style={{ marginRight: '10px', padding: '5px' }}
            />
            <select value={status} onChange={(e) => setStatus(e.target.value)} style={{ padding: '5px' }}>
                <option value="Operational">Operational</option>
                <option value="Warning">Warning</option>
                <option value="Maintenance">Maintenance</option>
            </select>
            <button type="submit" style={{ marginLeft: '10px', padding: '5px 10px', cursor: 'pointer' }}>Add</button>
        </form>
    );
}
