import React, { useRef, useEffect } from 'react';
import { useSystemClock } from '../context/useHooks.js';

export function Header({ title }) {
    const time = useSystemClock();
    // Point 3: useRef - Persistent value without re-rendering or DOM access
    const renderCount = useRef(0);

    useEffect(() => {
        renderCount.current += 1;
    });

    return (
        <header style={{
            padding: '20px 0',
            borderBottom: '2px solid #eee',
            display: 'flex',
            justifyContent: 'space-between',
            alignItems: 'center'
        }}>
            <div>
                <h1 style={{ margin: 0, color: '#2c3e50' }}>{title}</h1>
                <small style={{ color: '#7f8c8d' }}>System Status: Online | Render Cycle: {renderCount.current}</small>
            </div>
            <div style={{
                backgroundColor: '#2c3e50',
                color: 'white',
                padding: '10px 20px',
                borderRadius: '25px',
                fontWeight: 'bold',
                fontFamily: 'monospace'
            }}>
                🕒 {time}
            </div>
        </header>
    );
}

const WelcomeMessage = () => <p style={{ color: '#34495e' }}>Industrial Intelligence Gateway v2.0</p>;

export default WelcomeMessage;
