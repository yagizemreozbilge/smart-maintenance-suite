import React from 'react';
import { useMaintenance } from '../context/MaintenanceContext.js';
import { Box, AlertTriangle, CheckCircle } from 'lucide-react';

export function InventoryPanel() {
    const { inventory } = useMaintenance();

    return (
        <div style={{ padding: '15px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '20px' }}>
                <Box size={20} color="#e74c3c" />
                <h3 style={{ margin: 0 }}>Parts & Inventory</h3>
            </div>

            <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
                {inventory.map((item) => {
                    const isCritical = item.quantity <= item.min_stock;
                    return (
                        <div key={item.id} style={{
                            padding: '12px',
                            borderRadius: '8px',
                            backgroundColor: '#f8f9fa',
                            border: `1px solid ${isCritical ? '#fab1a0' : '#e1e4e8'}`,
                            display: 'flex',
                            justifyContent: 'space-between',
                            alignItems: 'center'
                        }}>
                            <div>
                                <div style={{ fontWeight: 'bold', fontSize: '0.9rem' }}>{item.part_name}</div>
                                <div style={{ fontSize: '0.7rem', color: '#7f8c8d' }}>SKU: {item.sku}</div>
                            </div>

                            <div style={{ textAlign: 'right' }}>
                                <div style={{
                                    display: 'flex',
                                    alignItems: 'center',
                                    gap: '5px',
                                    color: isCritical ? '#e17055' : '#27ae60',
                                    fontWeight: 'bold',
                                    fontSize: '1rem'
                                }}>
                                    {item.quantity}
                                    {isCritical ? <AlertTriangle size={14} /> : <CheckCircle size={14} />}
                                </div>
                                <div style={{ fontSize: '0.7rem', color: '#95a5a6' }}>Min: {item.min_stock}</div>
                            </div>
                        </div>
                    );
                })}
            </div>
        </div>
    );
}
