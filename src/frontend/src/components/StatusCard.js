// Point 1: Functional Component
// Point 5: Using children prop
export function StatusCard({ label, value, isLowStock, children }) {

    // Point 6: Conditional Rendering inside the component
    let statusColor;
    if (isLowStock) {
        statusColor = 'red';
    } else {
        statusColor = 'green';
    }

    return (
        // Point 3: JSX Markup, camelCase (className), Single Parent (div)
        // Point 4: Style with double curly braces {{ }}
        <div className="status-card" style={{ border: `1px solid ${statusColor}`, padding: '10px', margin: '5px' }}>
            <h3>{label}</h3>

            {/* Point 6: Ternary operator for conditional text */}
            <p>Current: {value} {isLowStock ? '( LOW STOCK)' : '(OK)'}</p>

            {/* Point 6: Logical && for optional content */}
            {isLowStock && <button onClick={() => alert('Restock triggered!')}>Restock Now</button>}

            {/* Point 5: Rendering children */}
            <div className="card-details">
                {children}
            </div>
        </div>
    );
}
