import React, { useState, useEffect, useRef } from 'react';
import { Clock, User } from 'lucide-react';

const WelcomeMessage = () => {
    const [time, setTime] = useState(new Date().toLocaleTimeString());

    useEffect(() => {
        const timer = setInterval(() => setTime(new Date().toLocaleTimeString()), 1000);
        return () => clearInterval(timer);
    }, []);

    return (
        <div style={{ padding: '10px 0', borderBottom: '1px solid #eee', marginBottom: '20px', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px', color: '#666' }}>
                <Clock size={16} />
                <span>{time}</span>
            </div>
            <div style={{ color: '#2ecc71', fontWeight: 'bold', fontSize: '0.9rem' }}>
                ● SYSTEM OPERATIONAL
            </div>
        </div>
    );
};

export const Header = ({ title }) => {
    const username = localStorage.getItem('username') || 'Unknown User';
    const role = localStorage.getItem('user_role') || 'operator';

    return (
        <header style={{
            display: 'flex',
            justifyContent: 'space-between',
            alignItems: 'center',
            padding: '20px 0',
            borderBottom: '2px solid #2c3e50',
            marginBottom: '10px'
        }}>
            <div>
                <h1 style={{ margin: 0, color: '#2c3e50', fontSize: '2rem', fontWeight: 800 }}>{title}</h1>
                <div style={{ color: '#7f8c8d', fontSize: '0.9rem', marginTop: '4px' }}>
                    INDUSTRIAL INTELLIGENCE GATEWAY V2.0
                </div>
            </div>

            <div style={{
                display: 'flex',
                alignItems: 'center',
                gap: '15px',
                backgroundColor: '#f8f9fa',
                padding: '10px 20px',
                borderRadius: '12px',
                border: '1px solid #e1e4e8'
            }}>
                <div style={{ textAlign: 'right' }}>
                    <div style={{ fontWeight: 'bold', color: '#2c3e50', fontSize: '1rem' }}>{username}</div>
                    <div style={{
                        fontSize: '0.7rem',
                        color: '#3498db',
                        fontWeight: 'bold',
                        textTransform: 'uppercase',
                        letterSpacing: '1px'
                    }}>
                        {role.toUpperCase()} LEVEL ACCESS
                    </div>
                </div>
                <div style={{
                    width: '40px',
                    height: '40px',
                    backgroundColor: '#2c3e50',
                    borderRadius: '50%',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    color: 'white'
                }}>
                    <User size={20} />
                </div>
            </div>
        </header>
    );
};

export default WelcomeMessage;
