import React from 'react';
import { Package, AlertTriangle, CheckCircle } from 'lucide-react';

export function StatusCard({ label, value, isLowStock, onRestock, children }) {
    const statusColor = isLowStock ? '#e74c3c' : '#2ecc71';

    return (
        <div className="status-card" style={{
            border: `none`,
            borderLeft: `6px solid ${statusColor}`,
            padding: '20px',
            marginBottom: '20px',
            borderRadius: '12px',
            backgroundColor: 'white',
            boxShadow: '0 4px 6px -1px rgba(0,0,0,0.1)'
        }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '15px' }}>
                <Package size={20} color="#34495e" />
                <h3 style={{ margin: 0, fontSize: '1.1rem', color: '#2c3e50' }}>{label}</h3>
            </div>

            <div style={{ fontSize: '1.8rem', fontWeight: '800', margin: '10px 0' }}>{value}</div>

            {isLowStock ? (
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', color: '#e74c3c', fontSize: '0.85rem', fontWeight: '600' }}>
                    <AlertTriangle size={16} /> LOW STOCK WARNING
                </div>
            ) : (
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', color: '#2ecc71', fontSize: '0.85rem', fontWeight: '600' }}>
                    <CheckCircle size={16} /> LEVELS OPTIMAL
                </div>
            )}

            {isLowStock && (
                <button
                    onClick={onRestock}
                    style={{
                        width: '100%',
                        marginTop: '15px',
                        backgroundColor: '#3498db',
                        color: 'white',
                        border: 'none',
                        padding: '12px',
                        borderRadius: '8px',
                        cursor: 'pointer',
                        fontWeight: 'bold'
                    }}
                >
                    Quick Restock
                </button>
            )}

            <div style={{ marginTop: '15px', paddingTop: '15px', borderTop: '1px solid #f0f0f0', color: '#7f8c8d', fontSize: '0.85rem' }}>
                {children}
            </div>
        </div>
    );
}
