import React, { useState, useRef } from 'react';
import { useMaintenanceDispatch } from '../context/MaintenanceContext.js';

export function MaintenanceForm() {
    const [name, setName] = useState('');
    const [status, setStatus] = useState('Operational');

    // Point 3: useRef - DOM focus
    const inputRef = useRef(null);

    const dispatch = useMaintenanceDispatch();

    const handleSubmit = (e) => {
        e.preventDefault();
        if (!name) {
            // Point 3: Direct DOM manipulation for UX
            inputRef.current.focus();
            return;
        }

        dispatch({
            type: 'ADD_MACHINE',
            payload: { id: Date.now(), name, status }
        });

        setName('');
        // Auto focus back to input for better workflow
        inputRef.current.focus();
    };

    return (
        <div style={{ backgroundColor: '#fff', padding: '20px', borderRadius: '8px', boxShadow: '0 2px 4px rgba(0,0,0,0.05)' }}>
            <h3 style={{ marginTop: 0 }}>Equipment Onboarding</h3>
            <form onSubmit={handleSubmit} style={{ display: 'flex', flexDirection: 'column', gap: '15px' }}>
                <div>
                    <label style={{ display: 'block', marginBottom: '5px', fontSize: '0.9em' }}>Machine Designation</label>
                    <input
                        ref={inputRef} // Point 3: Hooking the DOM
                        type="text"
                        placeholder="e.g. Laser Cutter X-1"
                        value={name}
                        onChange={(e) => setName(e.target.value)}
                        style={{
                            width: '100%',
                            padding: '10px',
                            borderRadius: '4px',
                            border: '1px solid #ddd',
                            boxSizing: 'border-box'
                        }}
                    />
                </div>

                <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '10px' }}>
                    <div>
                        <label style={{ display: 'block', marginBottom: '5px', fontSize: '0.9em' }}>Initial Status</label>
                        <select
                            value={status}
                            onChange={(e) => setStatus(e.target.value)}
                            style={{ width: '100%', padding: '10px', borderRadius: '4px', border: '1px solid #ddd' }}
                        >
                            <option value="Operational">Operational</option>
                            <option value="Warning">Warning</option>
                            <option value="Maintenance">Maintenance</option>
                        </select>
                    </div>
                    <button type="submit" style={{
                        alignSelf: 'end',
                        padding: '10px',
                        backgroundColor: '#27ae60',
                        color: 'white',
                        border: 'none',
                        borderRadius: '4px',
                        cursor: 'pointer',
                        fontWeight: 'bold'
                    }}>Register Asset</button>
                </div>
            </form>
        </div>
    );
}
