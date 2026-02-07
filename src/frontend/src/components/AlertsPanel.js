import React from 'react';
import { useMaintenance } from '../context/MaintenanceContext.js';
import { Bell, ShieldAlert, Info } from 'lucide-react';

export function AlertsPanel() {
    const { alerts } = useMaintenance();

    return (
        <div style={{ padding: '10px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '15px' }}>
                <Bell size={20} color="#e67e22" />
                <h3 style={{ margin: 0, fontSize: '1.2rem' }}>System Notifications</h3>
            </div>

            <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
                {alerts.map((alert) => (
                    <div key={alert.id} style={{
                        padding: '12px',
                        borderRadius: '8px',
                        backgroundColor: alert.severity === 'CRITICAL' ? '#fdf2f2' : '#f0f7ff',
                        border: `1px solid ${alert.severity === 'CRITICAL' ? '#fbd5d5' : '#d1e9ff'}`,
                        fontSize: '0.85rem'
                    }}>
                        <div style={{ display: 'flex', gap: '10px' }}>
                            {alert.severity === 'CRITICAL' ? (
                                <ShieldAlert size={18} color="#c81e1e" />
                            ) : (
                                <Info size={18} color="#1c64f2" />
                            )}
                            <div style={{ flex: 1 }}>
                                <div style={{
                                    fontWeight: 'bold',
                                    color: alert.severity === 'CRITICAL' ? '#9b1c1c' : '#1e429f',
                                    marginBottom: '2px'
                                }}>
                                    {alert.severity}
                                </div>
                                <div style={{ color: '#4b5563', lineHeight: '1.4' }}>{alert.message}</div>
                                <div style={{ marginTop: '8px', fontSize: '0.75rem', color: '#9ca3af' }}>
                                    {alert.created_at}
                                </div>
                            </div>
                        </div>
                    </div>
                ))}
            </div>
        </div>
    );
}
