import React, { useState } from 'react'; // Point 4: useState import
import WelcomeMessage, { Header } from './components/Header.js';
import { StatusCard } from './components/StatusCard.js';
import { MachineList } from './components/MachineList.js';

function App() {
    // Point 4: State (Hafıza)
    // [Değişken, Onu Güncelleyen Fonksiyon] = useState(Başlangıç Değeri)
    const [stock, setStock] = useState(3);
    const [filter, setFilter] = useState('All');

    // Static Data for demonstration (Point 2)
    const machinesData = [
        { id: 1, name: "CNC Lathe-01", status: "Operational" },
        { id: 2, name: "Robot Arm-04", status: "Warning" },
        { id: 3, name: "Heat Furnace", status: "Operational" },
    ];

    // Point 3: Event Handler (React'e teslim edilecek fonksiyon)
    const handleRestock = () => {
        // Point 4: Normal bir değişken olsa (stock = 10) ekran yenilenmezdi.
        // setStock ile hem değeri değiştiriyoruz hem de React'e "Yeniden çiz!" diyoruz.
        setStock(10);
        alert("Restock order placed. System state updated.");
    };

    return (
        <div style={{ fontFamily: 'Segoe UI, Tahoma, Geneva, Verdana, sans-serif', maxWidth: '800px', margin: '0 auto', padding: '20px' }}>
            <Header title="Smart Maintenance Control Suite" />
            <WelcomeMessage />

            <section style={{ marginTop: '20px', display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '20px' }}>

                {/* Module 1: Inventory (State & Props & Events) */}
                <div>
                    <StatusCard
                        label="Engine Coolant"
                        value={stock}
                        isLowStock={stock < 5}
                        onRestock={handleRestock} // Point 3: Fonksiyonun referansı iletiliyor
                    >
                        <p>Storage: Sector B-12</p>
                    </StatusCard>
                </div>

                {/* Module 2: Machines (Map & Filter & State Control) */}
                <div style={{ border: '1px solid #ccc', padding: '15px', borderRadius: '8px' }}>
                    <div style={{ marginBottom: '10px' }}>
                        {/* Point 3: Event for changing state */}
                        <button onClick={() => setFilter('All')}>All</button>
                        <button onClick={() => setFilter('Operational')}>Operational</button>
                        <button onClick={() => setFilter('Warning')}>Warnings</button>
                    </div>

                    <MachineList machines={machinesData} filterStatus={filter} />
                </div>

            </section>

            {/* Point 6: Conditional Rendering at root level */}
            <footer style={{ marginTop: '40px', borderTop: '1px dotted #aaa', paddingTop: '10px' }}>
                {stock < 5 ? (
                    <span style={{ color: 'red', fontWeight: 'bold' }}>SİSTEM DURUMU: DİKKAT GEREKİYOR</span>
                ) : (
                    <span style={{ color: 'green' }}>SİSTEM DURUMU: NORMAL</span>
                )}
            </footer>
        </div>
    );
}

export default App;
