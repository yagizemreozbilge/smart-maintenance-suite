import React, { useState } from 'react';
import './LoginPage.css';

const LoginPage = ({ onLogin }) => {
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');
    const [error, setError] = useState('');
    const [loading, setLoading] = useState(false);

    const handleSubmit = async (e) => {
        e.preventDefault();
        setError('');
        setLoading(true);

        try {
            // Backend'deki yeni API'mize istek atıyoruz
            // Not: Basitlik için GET kullanıyoruz, gerçekte POST olmalı
            const response = await fetch(`${import.meta.env.VITE_API_URL}/api/login?u=${username}&p=${password}`);
            const data = await response.json();

            if (data.success) {
                // Giriş başarılı! Token ve kullanıcı bilgisini locale kaydediyoruz
                localStorage.setItem('auth_token', data.token);
                localStorage.setItem('user_role', data.role);
                localStorage.setItem('username', data.username);

                onLogin(data); // App.js'e giriş yapıldığını haber ver
            } else {
                setError('Hatalı kullanıcı adı veya şifre');
            }
        } catch (err) {
            setError('Sunucuya bağlanılamadı. Backend çalışıyor mu?');
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="login-container">
            <div className="login-card">
                <h1>Smart HMS</h1>
                <p>Industrial Intelligence Gateway</p>

                <form onSubmit={handleSubmit}>
                    <div className="input-group">
                        <label>Kullanıcı Adı</label>
                        <input
                            type="text"
                            value={username}
                            onChange={(e) => setUsername(e.target.value)}
                            placeholder="admin"
                            required
                        />
                    </div>

                    <div className="input-group">
                        <label>Şifre</label>
                        <input
                            type="password"
                            value={password}
                            onChange={(e) => setPassword(e.target.value)}
                            placeholder="••••••••"
                            required
                        />
                    </div>

                    <button type="submit" className="login-button" disabled={loading}>
                        {loading ? 'SİSTEME GİRİLİYOR...' : 'SİSTEME GİRİŞ YAP'}
                    </button>

                    {error && <div className="error-message">{error}</div>}
                </form>
            </div>
        </div>
    );
};

export default LoginPage;
