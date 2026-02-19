import React from 'react';
import { useMaintenance } from '../context/MaintenanceContext.js';
import { History, User, Calendar, ClipboardList } from 'lucide-react';

export function MaintenanceHistory() {
    const { maintenanceLogs } = useMaintenance();

    return (
        <div style={{ padding: '15px', backgroundColor: 'white', borderRadius: '12px', marginTop: '20px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '20px' }}>
                <History size={20} color="#3498db" />
                <h3 style={{ margin: 0 }}>Maintenance History</h3>
            </div>

            <div style={{ maxHeight: '300px', overflowY: 'auto' }}>
                {(maintenanceLogs && Array.isArray(maintenanceLogs) && maintenanceLogs.length > 0) ? (
                    maintenanceLogs.map((log) => (
                        <div key={log.id} style={{
                            padding: '12px',
                            borderBottom: '1px solid #eee',
                            display: 'grid',
                            gridTemplateColumns: '120px 1fr',
                            gap: '15px'
                        }}>
                            <div style={{ fontSize: '0.8rem', color: '#7f8c8d' }}>
                                <div style={{ display: 'flex', alignItems: 'center', gap: '5px' }}>
                                    <Calendar size={12} /> {log.date}
                                </div>
                                <div style={{ display: 'flex', alignItems: 'center', gap: '5px', marginTop: '4px' }}>
                                    <User size={12} /> {log.performer}
                                </div>
                            </div>
                            <div>
                                <div style={{ fontWeight: '600', color: '#2c3e50', fontSize: '0.9rem' }}>
                                    {log.description}
                                </div>
                                <div style={{ fontSize: '0.75rem', color: '#95a5a6', marginTop: '5px' }}>
                                    <ClipboardList size={12} style={{ verticalAlign: 'middle', marginRight: '4px' }} />
                                    Machine ID: {log.machine_id}
                                </div>
                            </div>
                        </div>
                    ))
                ) : (
                    <p style={{ textAlign: 'center', color: '#95a5a6', padding: '20px' }}>No history records found.</p>
                )}
            </div>
        </div>
    );
}
