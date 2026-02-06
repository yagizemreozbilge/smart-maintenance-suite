import React from 'react';

// Point 1: Passing Props (label, value, isLowStock, onRestock)
// Point 5 (from your list): children prop is also used here
export function StatusCard({ label, value, isLowStock, onRestock, children }) {

    const statusColor = isLowStock ? '#ff4d4d' : '#2ecc71';

    return (
        <div className="status-card" style={{
            border: `2px solid ${statusColor}`,
            padding: '15px',
            margin: '10px',
            borderRadius: '8px',
            backgroundColor: '#f9f9f9'
        }}>
            <h3 style={{ margin: '0 0 10px 0' }}>{label}</h3>

            <p>Current Level: <strong>{value}</strong></p>

            {/* Point 6 (Conditional Rendering) */}
            {isLowStock ? (
                <p style={{ color: 'red', fontWeight: 'bold' }}>⚠️ CRITICAL: Stock is below threshold!</p>
            ) : (
                <p style={{ color: 'green' }}>✅ Status: Operational</p>
            )}

            {/* Point 3: Events (onClick) 
          Wait! We don't call the function like onRestock(), 
          we pass the reference so React calls it when clicked. */}
            {isLowStock && (
                <button
                    onClick={onRestock}
                    style={{
                        backgroundColor: '#3498db',
                        color: 'white',
                        border: 'none',
                        padding: '8px 12px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Order Restock
                </button>
            )}

            <div style={{ marginTop: '10px', fontStyle: 'italic', fontSize: '0.9em' }}>
                {children}
            </div>
        </div>
    );
}
