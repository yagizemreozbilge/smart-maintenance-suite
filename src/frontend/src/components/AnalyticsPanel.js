import React from 'react';
import {
    BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer,
    PieChart, Pie, Cell, Sector
} from 'recharts';
import { useMaintenance } from '../context/MaintenanceContext.js';

const COLORS = ['#2ecc71', '#f1c40f', '#e74c3c', '#3498db'];

export function AnalyticsPanel() {
    const { machines, inventory } = useMaintenance();

    // 1. Prepare Machine Status Data for Pie Chart
    const statusCounts = (machines || []).reduce((acc, m) => {
        const rawStatus = m.status || 'unknown';
        const status = rawStatus.charAt(0).toUpperCase() + rawStatus.slice(1);
        acc[status] = (acc[status] || 0) + 1;
        return acc;
    }, {});

    const pieData = Object.keys(statusCounts).map(status => ({
        name: status,
        value: statusCounts[status]
    }));

    // 2. Prepare Inventory Data for Bar Chart
    const barData = inventory.map(item => ({
        name: item.part_name,
        stock: item.quantity,
        min: item.min_stock_level
    }));

    return (
        <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '20px', marginTop: '20px' }}>
            {/* Machine Status Distribution */}
            <div style={{ backgroundColor: 'white', padding: '20px', borderRadius: '12px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)' }}>
                <h4 style={{ margin: '0 0 20px 0', color: '#2c3e50' }}>Asset Status Distribution</h4>
                <div style={{ width: '100%', height: 250 }}>
                    <ResponsiveContainer>
                        <PieChart>
                            <Pie
                                data={pieData}
                                cx="50%"
                                cy="50%"
                                innerRadius={60}
                                outerRadius={80}
                                paddingAngle={5}
                                dataKey="value"
                            >
                                {pieData.map((entry, index) => (
                                    <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
                                ))}
                            </Pie>
                            <Tooltip />
                            <Legend verticalAlign="bottom" height={36} />
                        </PieChart>
                    </ResponsiveContainer>
                </div>
            </div>

            {/* Inventory Stock Levels */}
            <div style={{ backgroundColor: 'white', padding: '20px', borderRadius: '12px', boxShadow: '0 4px 6px rgba(0,0,0,0.05)' }}>
                <h4 style={{ margin: '0 0 20px 0', color: '#2c3e50' }}>Inventory Levels vs Min Stock</h4>
                <div style={{ width: '100%', height: 250 }}>
                    <ResponsiveContainer>
                        <BarChart
                            data={barData}
                            layout="vertical"
                            margin={{ top: 5, right: 30, left: 40, bottom: 5 }}
                        >
                            <CartesianGrid strokeDasharray="3 3" />
                            <XAxis type="number" />
                            <YAxis dataKey="name" type="category" width={100} style={{ fontSize: '0.7rem' }} />
                            <Tooltip />
                            <Legend />
                            <Bar dataKey="stock" fill="#3498db" name="Current Stock" />
                            <Bar dataKey="min" fill="#e74c3c" name="Minimum Level" />
                        </BarChart>
                    </ResponsiveContainer>
                </div>
            </div>
        </div>
    );
}
